#ifndef UTENSOR_ARENA_ALLOCATOR_HPP
#define UTENSOR_ARENA_ALLOCATOR_HPP
#include "memoryManagementInterface.hpp"
#include <forward_list>

namespace uTensor {

#define BLOCK_INACTIVE (0 << 15)
#define BLOCK_ACTIVE (1 << 15);
#define BLOCK_ZERO_LENGTH 0

/**
 * Size allocated must be less than 2**15
 */
template<size_t size>
class localCircularArenaAllocator : public AllocatorInterface {
  private:
    class MetaHeader {
      public:
        uint16_t meta_data;
        Handle* hndl;
      public:
        MetaHeader() : meta_data(BLOCK_INACTIVE | BLOCK_ZERO_LENGTH), hndl(nullptr) {}
        MetaHeader(uint16_t sz) : meta_data(BLOCK_ACTIVE | sz), hndl(nullptr) {}
        void set_active() { meta_data |= BLOCK_ACTIVE; }
        void set_inactive() { meta_data &= (BLOCK_ACTIVE | 0x7FFF) ; }
        void set_hndl(Handle* handle) { hndl = handle; }
        void set_len(uint16_t sz) { meta_data &= 0x8000; meta_data |= (0x7FFF & sz); }
        uint16_t get_len() const { return meta_data & 0x7FFF; }
        bool is_active() const { return (meta_data & 0x8000) == BLOCK_ACTIVE; }
        bool is_bound() const { return (hndl != nullptr); }
        bool has_handle(Handle* target) const { return is_active && (hndl == target); }
    }
  private:
    uint16_t capacity;
    uint8_t _buffer[size];
    uint8_t* cursor;

    // Return the amount of free space at the tail
    uint16_t tail_capacity() { };

    MetaHeader _read_header(void* ptr) const {
      // First check if ptr in bounds
      if(ptr < _buffer || ptr > (_buffer + size)){
        //ERROR
      }
      // Read the header
      uint8_t* p = reinterpret_cast<uint8_t*>(ptr);
      MetaHeader hdr;
      // TODO error check this
      memcpy(&hdr, p - sizeof(MetaHeader), sizeof(MetaHeader));
      return hdr;
    }
    void _write_header(const MetaHeader& hdr, void* ptr){
      // First check if ptr in bounds
      if(ptr < _buffer || ptr > (_buffer + size)){
        //ERROR
      }
      uint8_t* p = reinterpret_cast<uint8_t*>(ptr);
      memcpy(p - sizeof(MetaHeader), &hdr, sizeof(MetaHeader));
    }

    uint8_t* begin() const { return _buffer + sizeof(MetaHeader); }
    uint8_t* end() const { return _buffer + size; }
  protected:
    virtual void _bind(void* ptr, Handle* hndl){
      MetaHeader hdr = _read_header(ptr);
      //Check if region is active
      if(!hdr.is_active()){
        //ERROR
      }
      hdr.set_hndl(hndl);
      _write_header(hdr, ptr);
    }

    virtual void _unbind(void* ptr, Handle* hndl){
      //teehee
      hndl->_ptr = nullptr;
      _bind(ptr, nullptr);
    }

    virtual bool _is_bound(void* ptr, Handle* hndl){
      MetaHeader hdr = _read_header(ptr);
      //Check if region is active
      if(!hdr.is_active()){
        //ERROR
      }
      return hdr.is_bound();
    }

    virtual bool _has_handle(Handle* hndl){
      uint8_t* m_cursor = begin();
      while(!(m_cursor > end())) {
        MetaHeader hdr = _read_header((void*) m_cursor);
        if(hdr.has_handle(hndl)){
          return true;
        }
        m_cursor += hdr.get_len();
      }
      return false;
    }

    virtual void* _allocate(size_t sz){
      uint8_t* loc = nullptr;
      // If make this capacity then have possibility of filling up
      if(sz > size){
        //ERROR
      }
      if(sz > ( (_buffer + size) - cursor)){
        //Allocate at beginning
        // Rebalance to make it less likely to overwrite a region
        // Overwriting allocated regions is a valid operation as long as the overwritten regions are invalidated
        rebalance();
      }
      MetaHeader hdr = _read_header((void*) cursor);
      if(hdr.is_active() || (hdr.get_len() > 0 && hdr.get_len() < sz)){
        //clear necessary chunks until enough space for current request + appended fragment header
        uint8_t* forward_cursor = cursor;
        while((forward_cursor - cursor) < sz){
          MetaHeader f_hdr = _read_header((void*) forward_cursor);
          deallocate((void*) forward_cursor);
        // Decide whether we need to insert a fragment header or not
          forward_cursor += f_hdr.get_len() + sizeof(MetaHeader);
          if((forward_cursor - cursor) > (sz + sizeof(MetaHeader))){
            f_hdr.set_inactive();
            f_hdr.set_hndl(nullptr);
            // set it to the free length
            f_hdr.set_len( forward_cursor - sizeof(MetaHeader) - cursor + sz);
            _write_header( f_hdr, (void*)(cursor + sz));
          }
        }
      }
      hdr.set_active();
      hdr.set_len(sz);
      hdr.set_hndl(nullptr);
      _write_header(hdr, (void*) cursor);
      loc = cursor;
      cursor += hdr.get_len() + sizeof(MetaHeader);
      return (void*)loc;
    }
    virtual void _deallocate(void* ptr){
      if(ptr){
        MetaHeader hdr = _read_header(ptr);
        hdr.set_inactive();
        if(hdr.is_bound()){
          _unbind(ptr, hdr.hndl);
        }
        _write_header(hdr, ptr);
      }
    }

  public:
    localCircularArenaAllocator() : capacity(size) {
      memset(_buffer, 0, size);
      cursor = begin();
    }

    /** This implementation of rebalance shifts all allocated chunks to the end of the buffer and inserts an inactive region at the start.
     * note: cursor gets moved to begin()
     */
    virtual bool rebalance() {
      //TODO WARNING rebalancing Allocator
      // Shift each chunk towards the end of the buffer
      uint16_t empty_chunk_len = (uint16_t)(end() - cursor);
      uint16_t allocated_amount = (uint16_t)(cursor - _buffer);
      // From the end, move byte by byte until everything is shifted
      cursor--;
      uint8_t* tail = &_buffer[size-1];
      for(uint16_t i = 0; i < allocated_amount; i++){
        *tail = *cursor;
        tail--;
        cursor--;
      }

      // Next scan forward from the shifted points and update any bound handles
      uint8_t* forward_cursor = _buffer + empty_chunk_len;
      while(forward_cursor < end()){
        MetaHeader hdr = _read_header((void*) forward_cursor);
        if(hdr.is_bound()){
          hdr.hndl->_ptr = (void*) forward_cursor;
        }
        forward_cursor += hdr.get_len() + sizeof(MetaHeader);
      }
      
      // Write new header to start
      cursor = begin();
      MetaHeader hdr(empty_chunk_len);
      hdr.set_inactive();
      _write_header(hdr, (void*)cursor);
    }

    virtual size_t available() { 
      return end() - cursor;
    }
};

// Note not actually complete
template<size_t sz>
class ArenaCircularAllocator {
    private:
        static localCircularArenaAllocator<sz> _allocator;
    public:
        static void* allocate(size_t size) { 
            if (size > _allocator.available())
                return NULL;

            void* p = _allocator.allocate(size);
            if (p == NULL)
                _allocator.rebalance(); 
        }
        static void  deallocate(void* p) { ... }
        static void  bind(void* ptr, utensor::Tensor* hndl) {
            _allocator.bind(ptr, hndl);
        }

};

} //end namespace
#endif

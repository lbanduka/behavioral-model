#ifndef _BM_RAS_H_
#define _BM_RAS_H_

#include <cassert>

#include <Judy.h>

class RandAccessUIntSet {
public:
  typedef uintptr_t mbr_t;

public:
  /* this code is basically copied from handle_mgr.h, maybe I should just write
     a wrapper for Judy once and for all */

  class const_iterator;

  class iterator
  {
    friend class const_iterator;
  
  public:
    iterator(RandAccessUIntSet *ras_mgr, mbr_t index)
      : ras_mgr(ras_mgr), index(index) {}
    
    mbr_t &operator*() {return index;}
    mbr_t *operator->() {return &index;}
    
    bool operator==(const iterator &other) const {
      return (ras_mgr == other.ras_mgr) && (index == other.index);
    }
    
    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }
    
    iterator& operator++() {
      int Rc_int;
      Word_t jindex = index;;
      J1N(Rc_int, ras_mgr->members, jindex);
      if(!Rc_int)
	index = -1;
      else
	index = jindex;
      return *this;
    }

    iterator operator++(int){
      // Use operator++()
      const iterator old(*this);
      ++(*this);
      return old;
    }

  private:
    RandAccessUIntSet *ras_mgr;
    mbr_t index;
  };

  class const_iterator
  {
  public:
    const_iterator(const RandAccessUIntSet *ras_mgr, mbr_t index)
      : ras_mgr(ras_mgr), index(index) {}

    const_iterator(const iterator &other)
      : ras_mgr(other.ras_mgr), index(other.index) {}
    
    const mbr_t &operator*() const {return index;}
    const mbr_t *operator->() const {return &index;}

    const_iterator& operator=(const const_iterator &other) {
      ras_mgr = other.ras_mgr;
      index = other.index;
      return *this;
    }

    bool operator==(const const_iterator &other) const {
      return (ras_mgr == other.ras_mgr) && (index == other.index);
    }
  
    bool operator!=(const const_iterator &other) const {
      return !(*this == other);
    }

    const const_iterator& operator++(){
      int Rc_int;
      Word_t jindex = index;
      J1N(Rc_int, ras_mgr->members, jindex);
      if(!Rc_int)
	index = -1;
      else
	index = jindex;
      return *this;
    }

    const const_iterator operator++(int){
      // Use operator++()
      const const_iterator old(*this);
      ++(*this);
      return old;
    }

  private:
    const RandAccessUIntSet *ras_mgr;
    mbr_t index;
  };


public:
  RandAccessUIntSet()
    : members((Pvoid_t) NULL) { }

  ~RandAccessUIntSet() {
    Word_t bytes_freed;
    J1FA(bytes_freed, members);
  }

  RandAccessUIntSet(const RandAccessUIntSet &other) = delete;
  RandAccessUIntSet &operator=(const RandAccessUIntSet &other) = delete;

  RandAccessUIntSet(RandAccessUIntSet &&other) = default;
  RandAccessUIntSet &operator=(RandAccessUIntSet &&other) = default;

  // returns 0 if already present (0 element added), 1 otherwise
  int add(mbr_t mbr) {
    int Rc;
    J1S(Rc, members, mbr);
    return Rc;
  }

  // returns 0 if not present (0 element removed), 1 otherwise
  int remove(mbr_t mbr) {
    int Rc;
    J1U(Rc, members, mbr);
    return Rc;
  }

  bool contains(mbr_t mbr) const {
    int Rc;
    J1T(Rc, members, mbr);
    return Rc;
  }

  size_t count() const {
    size_t nb;
    J1C(nb, members, 0, -1);
    return nb;
  }

  // n >= 0, 0 is first element in set
  mbr_t get_nth(size_t n) const {
    assert(n <= count());
    int Rc;
    mbr_t mbr;
    J1BC(Rc, members, n + 1, mbr);
    assert(Rc == 1);
    return mbr;
  }

  // iterators

  iterator begin() {
    Word_t index = 0;
    int Rc_int;
    J1F(Rc_int, members, index);
    if(!Rc_int) index = -1;
    return iterator(this, index);
  }

  const_iterator begin() const {
    Word_t index = 0;
    int Rc_int;
    J1F(Rc_int, members, index);
    if(!Rc_int) index = -1;
    return const_iterator(this, index);
  }

  iterator end() {
    Word_t index = -1;
    return iterator(this, index);
  }

  const_iterator end() const {
    Word_t index = -1;
    return const_iterator(this, index);
  }

private:
  Pvoid_t members;
};

#endif

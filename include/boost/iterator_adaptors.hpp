// (C) Copyright David Abrahams 2000. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
//
// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#ifndef BOOST_ITERATOR_ADAPTOR_DWA053000_HPP_
# define BOOST_ITERATOR_ADAPTOR_DWA053000_HPP_

// MSVC complains about the wrong stuff unless you disable this. We should add
// this to config.hpp

#include <boost/operators.hpp>
#include <iterator>

namespace boost {

// Just a "type envelope"; works around some MSVC deficiencies.
template <class T>
struct type {};

//=============================================================================
// Default policies for iterator adaptors. You can use this as a base
// class if you want to customize particular policies.
struct default_iterator_policies
{
    template <class Reference, class Iterator>
    static Reference dereference(type<Reference>, const Iterator& x)
        { return *x; }

    template <class Iterator>
    static void increment(Iterator& x)
        { ++x; }

    template <class Iterator>
    static void decrement(Iterator& x)
        { --x; }

    template <class Iterator, class DifferenceType>
    static void advance(Iterator& x, DifferenceType n)
        { x += n; }

    template <class Difference, class Iterator1, class Iterator2>
    static Difference distance(type<Difference>, Iterator1& x, Iterator2& y)
        { return y - x; }

    template <class Iterator1, class Iterator2>
    static bool equal(Iterator1& x, Iterator2& y)
        { return x == y; }

    template <class Iterator1, class Iterator2>
    static bool less(Iterator1& x, Iterator2& y)
        { return x < y; }
};

//=============================================================================
// iterator_adaptor - A generalized adaptor around an existing
//   iterator, which is itself an iterator
//
//   Iterator - the iterator type being wrapped.
//
//   Policies - a set of policies determining how the resulting iterator
//      works.
//
//   Traits - a class satisfying the same requirements as a specialization of
//      std::iterator_traits for the resulting iterator.
//
//   NonconstIterator - the corresponding non-const iterator type for
//      Iterator, if any. You don't need to supply this if you are not make a
//      const/non-const iterator pair.
//
template <class Iterator, class Policies, 
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
          class Traits,
#else
          class Traits = std::iterator_traits<Iterator>,
#endif
          class NonconstIterator = Iterator
         >
struct iterator_adaptor
    : boost::iterator<typename Traits::iterator_category, typename Traits::value_type, typename Traits::difference_type, typename Traits::pointer, typename Traits::reference>
{
    typedef iterator_adaptor<Iterator, Policies, Traits, NonconstIterator> Self;
public:
    typedef typename Traits::difference_type difference_type;
    typedef typename Traits::value_type value_type;
    typedef typename Traits::pointer pointer;
    typedef typename Traits::reference reference;
    typedef typename Traits::iterator_category iterator_category;

    iterator_adaptor(const Iterator& impl)
        : m_impl(impl) {}

    template <class OtherTraits>
    iterator_adaptor(const iterator_adaptor<NonconstIterator, Policies, OtherTraits, NonconstIterator>& rhs)
        : m_impl(rhs.m_impl) {}

    template <class OtherTraits>
    Self& operator=(const iterator_adaptor<NonconstIterator, Policies, OtherTraits, NonconstIterator>& rhs)
        { m_impl = rhs.m_impl; return *this; }
    
    reference operator*() const {
        return Policies::dereference(type<reference>(), m_impl);
    }

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning( disable : 4284 )
#endif

    pointer operator->() const
        { return &*this; }

#ifdef _MSC_VER
# pragma warning(pop)
#endif

    reference operator[](difference_type n)
        { return *(*this + n); }
    
    Self& operator++() {
        Policies::increment(m_impl);
        return *this;
    }

    Self& operator++(int) { Self tmp(*this); ++*this; return tmp; }
    
    Self& operator--() {
        Policies::decrement(m_impl);
        return *this;
    }
    
    Self& operator--(int) { Self tmp(*this); --*this; return tmp; }

    Self& operator+=(difference_type n) {
        Policies::advance(m_impl, n);
        return *this;
    }
  
    Self& operator-=(difference_type n) {
        Policies::advance(m_impl, -n);
        return *this;
    }

private:
    typedef Policies policies_type;
public: // too many compilers have trouble when this is private.
    Iterator m_impl;
};

template <class Iterator, class Policies, class Traits, class NonconstIterator>
iterator_adaptor<Iterator,Policies,Traits,NonconstIterator>
operator-(iterator_adaptor<Iterator,Policies,Traits,NonconstIterator> p, const typename Traits::difference_type x)
{
    return p -= x;
}

template <class Iterator, class Policies, class Traits, class NonconstIterator>
iterator_adaptor<Iterator,Policies,Traits,NonconstIterator>
operator+(iterator_adaptor<Iterator,Policies,Traits,NonconstIterator> p, const typename Traits::difference_type x)
{
    return p += x;
}

template <class Iterator, class Policies, class Traits, class NonconstIterator>
iterator_adaptor<Iterator,Policies,Traits,NonconstIterator>
operator+(const typename Traits::difference_type x, iterator_adaptor<Iterator,Policies,Traits,NonconstIterator> p)
{
    return p += x;
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
typename Traits1::difference_type operator-(
    const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x,
    const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y )
{
    typedef typename Traits1::difference_type difference_type;
    return Policies::distance(type<difference_type>(), y.m_impl, x.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator==(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return Policies::equal(x.m_impl, y.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator<(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return Policies::less(x.m_impl, y.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator>(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x,
          const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) { 
    return Policies::less(y.m_impl, x.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator>=(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return !Policies::less(x.m_impl, y.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator<=(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x,
           const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return !Policies::less(y.m_impl, x.m_impl);
}

template <class Iterator1, class Iterator2, class Policies, class Traits1, class Traits2, class NonconstIterator>
inline bool 
operator!=(const iterator_adaptor<Iterator1,Policies,Traits1,NonconstIterator>& x, 
           const iterator_adaptor<Iterator2,Policies,Traits2,NonconstIterator>& y) {
    return !Policies::equal(x.m_impl, y.m_impl);
}

//=============================================================================
// iterator_adaptors - A type generator that simplifies creating
//   mutable/const pairs of iterator adaptors.

template <class Iterator, class ConstIterator, 
#ifdef BOOST_NO_STD_ITERATOR_TRAITS
          class Traits, 
          class ConstTraits, 
#else
          class Traits = std::iterator_traits<Iterator>, 
          class ConstTraits = std::iterator_traits<ConstIterator>, 
#endif
          class Policies = default_iterator_policies>
class iterator_adaptors
{
public:
    typedef iterator_adaptor<Iterator, Policies, Traits, Iterator> iterator;
    typedef iterator_adaptor<ConstIterator, Policies, ConstTraits, Iterator> 
      const_iterator;
};


//=============================================================================
// Transform Iterator Adaptor

template <class AdaptableUnaryFunction>
struct transform_iterator_policies : public default_iterator_policies
{
    transform_iterator_policies() { }
    transform_iterator_policies(const AdaptableUnaryFunction& f) : m_f(f) { }
    
    template <class Reference, class Iterator>
    Reference dereference(type<Reference>, const Iterator& x) const
        { return m_f(*x); }

    AdaptableUnaryFunction m_f;
};

template <class AdaptableUnaryFunction, class IteratorTraits>
struct transform_iterator_traits {
    typedef typename AdaptableUnaryFunction::result_type value_type;
    typedef value_type reference;
    typedef value_type* pointer;
    typedef typename IteratorTraits::difference_type difference_type;
    typedef typename IteratorTraits::iterator_category iterator_category;
};
  
template <class AdaptableUnaryFunction,
          class Iterator,
#ifndef BOOST_NO_ITERATOR_TRAITS
          class Traits = std::iterator_traits<Iterator>
#else
          class Traits
#endif
         >
struct transform_iterator
{
    typedef transform_iterator_traits<AdaptableUnaryFunction,Traits>
      TransTraits;
    typedef iterator_adaptor<Iterator, 
      transform_iterator_policies<AdaptableUnaryFunction>, TransTraits, 
      Iterator> type;
};


//=============================================================================
// Indirect Iterators Adaptor

// Tried implementing this with transform_iterator, but that required
// using boost::remove_ref, which is not compiler portable.

struct indirect_iterator_policies : public default_iterator_policies
{
    template <class Reference, class Iterator>
    Reference dereference(type<Reference>, const Iterator& x) const
        { return **x; }
};

template <class IndirectIterator,
#ifdef BOOST_NO_ITERATOR_TRAITS
          class IndirectTraits,
          class Traits
#else
          class IndirectTraits = std::iterator_traits<IndirectIterator>,
          class Traits = 
            std::iterator_traits<typename IndirectTraits::value_type>
#endif
       >
struct indirect_traits
{
    typedef typename IndirectTraits::difference_type difference_type;
    typedef typename Traits::value_type value_type;
    typedef typename Traits::pointer pointer;
    typedef typename Traits::reference reference;
    typedef typename IndirectTraits::iterator_category iterator_category;
};

template <class IndirectIterator, class ConstIndirectIterator,
#ifdef BOOST_NO_ITERATOR_TRAITS
          class IndirectTraits,
          class ConstIndirectTraits,
          class Traits
#else
          class IndirectTraits = 
              std::iterator_traits<IndirectIterator>,
          class ConstIndirectTraits = 
              std::iterator_traits<ConstIndirectIterator>,
          class Traits =
              std::iterator_traits<typename IndirectTraits::value_type>
#endif
           >
struct indirect_iterators
{
    typedef typename IndirectTraits::value_type Iterator;
    typedef typename Traits::value_type ValueType;
    typedef iterator_adaptors<IndirectIterator, ConstIndirectIterator,
        indirect_traits<IndirectIterator, IndirectTraits, Traits>,
        indirect_traits<ConstIndirectIterator, ConstIndirectTraits, Traits>,
        indirect_iterator_policies
        > Adaptors;
    typedef typename Adaptors::iterator iterator;
    typedef typename Adaptors::const_iterator const_iterator;
};


//=============================================================================
// Reverse Iterators Adaptor

struct reverse_iterator_policies
{
    template <class Reference, class Iterator>
    Reference dereference(type<Reference>, const Iterator& x) const
        { return *boost::prior(x); }
    
    template <class Iterator>
    void increment(Iterator& x) const
        { --x; }
    
    template <class Iterator>
    void decrement(Iterator& x) const
        { ++x; }
    
    template <class Iterator, class DifferenceType>
    void advance(Iterator& x, DifferenceType n) const
        { x -= n; }
    
    template <class Difference, class Iterator1, class Iterator2>
    Difference distance(type<Difference>, Iterator1& x, Iterator2& y) const
        { return x - y; }
    
    template <class Iterator1, class Iterator2>
    bool equal(Iterator1& x, Iterator2& y) const
        { return x == y; }
    
    template <class Iterator1, class Iterator2>
    bool less(Iterator1& x, Iterator2& y) const
        { return y < x; }
};
  
template <class Iterator, class ConstIterator,
#ifndef BOOST_NO_ITERATOR_TRAITS
          class Traits = std::iterator_traits<Iterator>, 
          class ConstTraits = std::iterator_traits<ConstIterator>
#else
          class Traits,
          class ConstTraits
#endif
         >
struct reverse_iterators
{
    typedef iterator_adaptors<Iterator,ConstIterator,Traits,ConstTraits,
        reverse_iterator_policies> Adaptor;
    typedef typename Adaptor::iterator iterator;
    typedef typename Adaptor::const_iterator const_iterator;
};

//=============================================================================
// Counting Iterator and Integer Range Class

struct counting_iterator_policies : public default_iterator_policies
{
    template <class IntegerType>
    IntegerType dereference(type<IntegerType>, const IntegerType& i) const
        { return i; }
};
template <class IntegerType>
struct counting_iterator_traits {
    typedef IntegerType value_type;
    typedef IntegerType reference;
    typedef value_type* pointer;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
};

template <class IntegerType>
struct integer_range {
    typedef iterator_adaptor<IntegerType, counting_iterator_policies,
      counting_iterator_traits<IntegerType>, IntegerType> iterator;
    typedef iterator const_iterator;
    typedef IntegerType value_type;
    typedef std::ptrdiff_t difference_type;
    typedef IntegerType reference;
    typedef IntegerType const_reference;
    typedef const IntegerType* pointer;
    typedef const IntegerType* const_pointer;
    typedef IntegerType size_type;

    integer_range(IntegerType start, IntegerType finish)
        : m_start(start), m_finish(finish) { }

    iterator begin() const { return iterator(m_start); }
    iterator end() const { return iterator(m_finish); }
    size_type size() const { return m_finish - m_start; }
    bool empty() const { return m_finish == m_start; }
    void swap(integer_range& x) {
        std::swap(m_start, x.m_start);
        std::swap(m_finish, x.m_finish);
    }
protected:
    IntegerType m_start, m_finish;
};


} // namespace boost

#endif


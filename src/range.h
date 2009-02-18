#ifndef RANGE_H__INCLUDED
#define RANGE_H__INCLUDED

#include <iostream>

#include <boost/array.hpp>
#include <boost/operators.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <range1.h>
#include <coordinates.h>
#include <iterator.h>

namespace TiledArray {

  // need these forward declarations
  template<unsigned int DIM, typename CS> class Range;
  template<unsigned int DIM, typename CS> std::ostream& operator<<(std::ostream& out,
                                                                   const Range<DIM,CS>& rng);


  /// Range is a tiled DIM-dimensional range. It is immutable, to simplify API.
  template<unsigned int DIM, typename CS = CoordinateSystem<DIM> > class Range :
      boost::equality_comparable1< Range<DIM,CS> > {

      typedef Range<DIM,CS> my_type;
      typedef boost::array<Range1,DIM> Ranges;
      typedef Range1::element_index index_t;
      struct ElementTag {
      };
      struct TileTag {
      };

    public:
      // typedefs
      typedef CS CoordinateSystem;
      typedef ArrayCoordinate<index_t,DIM,TileTag,CoordinateSystem> tile_index;
      typedef ArrayCoordinate<index_t,DIM,ElementTag,CoordinateSystem> element_index;
      typedef size_t ordinal_index;

      // ready to declare iterators
      /// iterates over Range1
      typedef typename Ranges::const_iterator range_iterator;
      /// iterates over tile indices
      typedef detail::IndexIterator< tile_index, my_type > tile_iterator;
      /// iterates over tile indices
      typedef detail::IndexIterator< element_index, my_type > element_iterator;
/*
      /// A DIM-dimensional tile
      class Tile : boost::equality_comparable1<Tile> {
          typedef Tile my_type;
          typedef typename Range::element_index element_index;
          typedef typename Range::tile_index tile_index;
      public:
          typedef detail::IndexIterator<element_index,my_type> element_iterator;

      private:
          /// first index
          element_index start_;
          /// past-last index, i.e. last + 1
          element_index finish_;
          /// Current tile index
          tile_index index_;

          void throw_if_invalid() {
            if (finish() < start() )
              abort();
          }

          element_index start() const {
            return start_;
          }

          element_index finish() const {
            return finish_;
          }

          tile_index index() const {
        	  return index_;
          }

        public:
          Tile() {
          }

          Tile(element_index start, element_index finish, tile_index idx) :
            start_(start), finish_(finish), index_(idx) {
            throw_if_invalid();
          }

          template <typename Tile1ContainerIterator>
          Tile(const Tile1ContainerIterator& tbegin,
               const Tile1ContainerIterator& tend)
          {
            typename element_index::iterator start_iterator = start_.begin();
            typename element_index::iterator finish_iterator = finish_.begin();
            for(Tile1ContainerIterator d=tbegin; d!=tend; ++d, ++start_iterator, ++finish_iterator) {
              *start_iterator = * d->begin();
              *finish_iterator = * d->end();
            }
          }

          Tile(const Tile& src) :
            start_(src.start_), finish_(src.finish_) {
          }

          const Tile& operator=(const Tile& src) {
            start_ = src.start_;
            finish_ = src.finish_;
            return *this;
          }

          bool operator==(const Tile& A) const {
            return start_ == A.start_ && finish_ == A.finish_ && index_ == A.index_;
          }

          typename element_index::volume size() const {
            return volume(finish() - start());
          }

          void print(std::ostream& out) const {
            out << "Range<" << DIM << ">::Tile(@= " << this;
            out << " start=" << start() << " finish=" << finish()
            << " size=" << size() << " )";
          }

          element_iterator begin() const {
            element_iterator result(start(),*this);
            return result;
          }
          element_iterator end() const {
            element_iterator result(finish(),*this);
            return result;
          }

          void increment(element_index& i) const {
            // TODO implemented iterator over Range<D>::Tile
            abort();
          }
      };
*/
      // Default constructor
      Range() {
        init_();
      }

      // Constructed with an array of ranges
      Range(const Range1* ranges) {
        std::copy(ranges, ranges + DIM, ranges_.begin());
        init_();
      }

      /// Constructor from an iterator range of Range1
      template <typename RangeIterator>
      Range(const RangeIterator& ranges_begin, const RangeIterator& ranges_end) {
        std::copy(ranges_begin, ranges_end, ranges_.begin());
        init_();
      }

      /// Returns an iterator pointing to the first element
      element_iterator begin_element() const {
        return element_iterator(start_element(), *this);
      }

      /// Return an iterator pointing to the one past the last element
      element_iterator end_element() const {
        return element_iterator(finish_element(), *this);
      }

      /// Returns an iterator pointing to the first range.
      tile_iterator begin_tile() const {
        tile_iterator result(start_tile(), *this);
        return result;
      }

      /// Return an iterator pointing one past the last dimension.
      tile_iterator end_tile() const {
        return tile_iterator(finish_tile(), *this);
      }

      /// Increment tile index.
      void increment(tile_index& t) const {
        // Get order iterators.
        typename detail::DimensionOrder<DIM>::const_iterator order_iter = CoordinateSystem::ordering().begin();
        const typename detail::DimensionOrder<DIM>::const_iterator end_iter = CoordinateSystem::ordering().end();

        // increment least significant, and check to see if the iterator has reached the end
    	for(; order_iter != end_iter; ++order_iter) {
          // increment and break if done.
          if( (++(t[*order_iter]) ) < finish_tile()[*order_iter])
            return;

          // Reset current index to start value.
          t[*order_iter] = start_tile()[*order_iter];
        }

    	// Check for end (i.e. t was reset to start)
    	if(t == start_tile())
          t = finish_tile();
      }

      /// Increment element index.
      void increment(element_index& e) const {
        typename detail::DimensionOrder<DIM>::const_iterator order_iter = CoordinateSystem::ordering().begin();
        const typename detail::DimensionOrder<DIM>::const_iterator end_iter = CoordinateSystem::ordering().end();

        // increment least significant, and check to see if the iterator has reached the end
    	for(; order_iter != end_iter; ++order_iter) {
          // increment and break if done.
          if( (++(e[*order_iter]) ) < finish_element()[*order_iter])
            return;

          // Reset current index to start value.
          e[*order_iter] = start_element()[*order_iter];
        }

    	// Check for end (i.e. t was reset to start)
    	if(e == start_element())
          e = finish_element();
      }

      /// Returns an iterator pointing to the first range.
      range_iterator begin_range() const {
        return ranges_.begin();
      }

      /// Return an iterator pointing one past the last dimension.
      range_iterator end_range() const {
        return ranges_.end();
      }

      /// number of elements
      ordinal_index size() const {
        return nelems_;
      }

      /// number of elements of tile
      ordinal_index size(const tile_index& t) const {
    	  ordinal_index n = 1;
    	  for(size_t d = 0; d < DIM; ++d)
            n *= ranges_[d].size(t[d]);

    	  return n;
      }

      /// number of tiles
      ordinal_index ntiles() const {
        return ntiles_;
      }

      /// Returns true if element_index is within the range
      bool includes(const element_index& e) const {
        for(unsigned int d = 0; d < DIM; ++d)
          if ( !ranges_[d].includes_element(e[d]) )
            return false;
        return true;
      }

      /// Returns true if tile_index is within the range
      bool includes(const tile_index& t) const {
        for(unsigned int d=0; d<DIM; ++d)
          if ( !ranges_[d].includes_tile(t[d]) )
            return false;
        return true;
      }

      /// Return the tile that contains an element index.
      tile_iterator find(const element_index e) {
        tile_index tmp;

        for (unsigned int dim = 0; dim < DIM; ++dim)
          tmp[dim] = *( ranges_[dim].find(e[dim]) );

        if (includes(tmp)) {
          tile_iterator result(tmp,*this);
          return result;
        }
        else
          return end_tile();
      }

/*
      /// Returns Tile for the given tile_index t. Assumes that includes(t) is true.
      Tile tile(const tile_index& t) const {
        boost::array<Range1::Tile,DIM> tiles1;
        for(unsigned int d=0; d<DIM; ++d)
          tiles1[d] = ranges_[d].tile(t[d]);
        Tile result(tiles1.begin(),tiles1.end());
        return result;
      }
*/

      Range<DIM>& permute(const Permutation<DIM>& perm) {
        operator^(perm, ranges_);
        init_();
        return *this;
      }

      // Equality operator
      bool operator ==(const Range<DIM>& rng) const {
        if(&rng == this)
          return true;
        else
          return std::equal(begin_range(), end_range(), rng.begin_range());
      }

      /// return element with the smallest indices in each dimension
      element_index start_element() const {
        return start_element_;
      }

      /// return element past the one with the largest indices in each dimension
      element_index finish_element() const {
        return finish_element_;
      }

      /// return element with the smallest indices in each dimension
      element_index start_element(const tile_index& t) const {
        assert(includes(t));
        element_index result;
    	for(size_t d = 0; d < DIM; ++d)
          result[d] = ranges_[d].start_element(t[d]);
        return result;
      }

      /// return element past the one with the largest indices in each dimension
      element_index finish_element(const tile_index& t) const {
        assert(includes(t));
    	element_index result;
        for(size_t d = 0; d < DIM; ++d)
          result[d] = ranges_[d].finish_element(t[d]);
        return result;
      }

      /// return tile with the smallest indices in each dimension
      tile_index start_tile() const {
        return start_tile_;
      }

      /// return tile past the one with the largest indices in each dimension
      tile_index finish_tile() const {
        return finish_tile_;
      }


    private:
      Ranges ranges_; // Vector of range data for each dimension

      /// precomputes useful data listed below
      void init_() {
        // Get dim ordering iterator
        const detail::DimensionOrder<DIM>& dimorder = CoordinateSystem::ordering();
        typename detail::DimensionOrder<DIM>::const_iterator d;

        ordinal_index tile_weight = 1;
        ordinal_index element_weight = 1;
        for(d = dimorder.begin(); d != dimorder.end(); ++d) {
          // Init ordinal weights for tiles.
          tile_ordinal_weights_[*d] = tile_weight;
          tile_weight *= ranges_[*d].ntiles();

          // init ordinal weights for elements.
          element_ordinal_weights_[*d] = element_weight;
          element_weight *= ranges_[*d].size();
        }
        nelems_ = element_weight;
        ntiles_ = tile_weight;

        // init start & finish for elements and tiles
        for(unsigned int d=0; d<DIM; ++d) {
          // Store upper and lower element ranges
          start_element_[d] = ranges_[d].start_element();
          finish_element_[d] = ranges_[d].finish_element();
          // Store upper and lower tile ranges
          start_tile_[d] = ranges_[d].start_tile();
          finish_tile_[d] = ranges_[d].finish_tile();
        }
      }

      /// computes an ordinal index for a given tile_index
      ordinal_index ordinal(const element_index& e) {
    	assert(includes(e));
    	element_index relative_index = e - start_element();
        ordinal_index result = dot_product(relative_index.data(),element_ordinal_weights_);
        return result;
      }

      /// computes an ordinal index for a given tile_index
      ordinal_index ordinal(const tile_index& t) {
        assert(includes(t));
        tile_index relative_index = t - start_tile();
        ordinal_index result = dot_product(relative_index.data(),tile_ordinal_weights_);
        return result;
      }

      /// to compute ordinal dot tile_index with this.
      /// this automatically takes care of dimension ordering
      boost::array<ordinal_index,DIM> tile_ordinal_weights_;
      boost::array<ordinal_index,DIM> element_ordinal_weights_;
      ordinal_index ntiles_;
      ordinal_index nelems_;
      element_index start_element_;
      element_index finish_element_;
      tile_index start_tile_;
      tile_index finish_tile_;

      friend std::ostream& operator << <>(std::ostream&, const Range& rng);

  };

  template<unsigned int DIM, typename CS> std::ostream& operator<<(std::ostream& out,
                                                                   const Range<DIM,CS>& rng) {
    out << "Range<" << DIM << ">(" << " @= " << &rng
        << " *begin_tile=" << (*rng.begin_tile()) << " *end_tile=" << (*rng.end_tile())
        << " *begin_element=" << (*rng.begin_element()) << " *end_element=" << (*rng.end_element())
        << " size=" << rng.size() << " ntiles=" << rng.ntiles() << " )";
    return out;
  }

  template<unsigned int DIM, typename CS> std::ostream& print(std::ostream& out,
                                                              const typename Range<DIM>::Tile& tile) {
    tile.print(out);
    return out;
  }

}
; // end of namespace TiledArray


#endif // RANGE_H__INCLUDED

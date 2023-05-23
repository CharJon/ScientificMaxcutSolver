#ifndef SMS_COMPACT_GRAPH_HPP
#define SMS_COMPACT_GRAPH_HPP

#include <concepts>
#include <span>
#include <ranges>
#include <iterator>

#include "networkit/graph/Graph.hpp"

using edgeweight = double;

// We opt for signed integers, for reasons similar to
// what Bjarne Stroustrup and others think about the unsigned type in general
// See e.g. [1] and references there in.
// [1]: https://hamstergene.github.io/posts/2021-10-30-do-not-use-unsigned-for-nonnegativity/

template<typename T> requires std::signed_integral<T>
struct WeightedEdge {
    T u, v;
    edgeweight weight;
    T id;

    WeightedEdge(T _u, T _v, edgeweight _weight, bool sorted = false, T _id = -1) {
        u = sorted ? std::min(_u, _v) : _u;
        v = sorted ? std::max(_u, _v) : _v;
        weight = _weight;
        id = _id;
    }
};


template<typename T> requires std::signed_integral<T>
class CompactGraph {

public:
    CompactGraph() :
            n_(0), m_(0) {
        indexArray_ = std::vector<T>();
        edgeArray_ = std::vector<WeightedEdge<T>>();
    }

    explicit CompactGraph(const NetworKit::Graph &g) : n_(g.upperNodeIdBound()), m_(g.numberOfEdges()) {
        indexArray_ = std::vector<T>(n_ + 1, 0);
        edgeArray_ = std::vector<WeightedEdge<T>>(m_ * 2, {0, 0, 0});

        edgeIDmapping_ = std::vector<T>(m_ * 2, -1);

        for (T u = 0; u < n_; u++) {
            indexArray_[u + 1] = indexArray_[u] + g.degree(u);
        }

        std::vector<T> posArray(indexArray_);

        for (auto e: g.edgeRange()) {
            T u = e.u;
            T v = e.v;
            edgeweight w = g.weight(u, v);
            edgeArray_[posArray[u]] = {u, v, w, false, posArray[u]};
            edgeArray_[posArray[v]] = {v, u, w, false, posArray[v]};

            edgeIDmapping_[posArray[u]] = posArray[v];
            edgeIDmapping_[posArray[v]] = posArray[u];

            posArray[u]++;
            posArray[v]++;
        }
    }

    T numberOfNodes() const {
        return n_;
    }

    T numberOfEdges() const {
        return m_;
    }

    T degree(T u) const {
        return indexArray_[u + 1] - indexArray_[u];
    }

    std::span<const WeightedEdge<T>> neighborRange(T u) const {
        auto start = indexArray_[u];
        auto end = indexArray_[u + 1];

        return {edgeArray_.begin() + start, edgeArray_.begin() + end};
    }

    std::ranges::iota_view<T, T> nodeRange() const {
        return {0, n_};
    }

    auto edgeRange() const {
        return edgeArray_ | std::views::filter([](auto e) { return e.u < e.v; });
    }

    const std::vector<WeightedEdge<T>> &edgeArray() const {
        return edgeArray_;
    }

    const std::vector<T> &edgeIDmapping() const {
        return edgeIDmapping_;
    }

    T edgeId(T u, T v) const {

        auto start = indexArray_[u];
        auto end = indexArray_[u + 1];

        for (auto i = start; i < end; i++) {
            if (edgeArray_[i].v == v) {
                assert(edgeArray_[i].id == i);
                return edgeArray_[i].id;
            }
        }
        return -1;
    }


private:
    T n_;
    T m_;
    std::vector<T> indexArray_;
    std::vector<T> edgeIDmapping_;
    std::vector<WeightedEdge<T>> edgeArray_;

};


#endif //SMS_COMPACT_GRAPH_HPP

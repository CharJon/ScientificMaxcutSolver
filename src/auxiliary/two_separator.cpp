#include "sms/auxiliary/two_separator.hpp"


#include "networkit/graph/Graph.hpp"
#include "networkit/components/BiconnectedComponents.hpp"
#include "networkit/components/DynConnectedComponents.hpp"

bool TwoSeparator::hasRun() const
{
    return run_;
}

void TwoSeparator::run()
{
    Graph g_copy(graph);

    NetworKit::DynConnectedComponents generator(g_copy);
    generator.run();

    for (auto u: graph.nodeRange())
    {
        assert(g_copy.numberOfNodes() == graph.numberOfNodes());
        assert(g_copy.numberOfEdges() == graph.numberOfEdges());

        g_copy.removeNode(u);

        for (auto v: graph.nodeRange())
        {
            if (u < v)
            {
                g_copy.removeNode(v);

                assert(g_copy.numberOfNodes() == graph.numberOfNodes() - 2);

                generator.run();

                if (generator.numberOfComponents() > 1)
                    separators.emplace_back(u, v);

                //restore node v
                g_copy.restoreNode(v);

                for (auto n: graph.neighborRange(v))
                {
                    if (n != u)
                        g_copy.addEdge(v, n);
                }
            }
        }

        //restore node u
        g_copy.restoreNode(u);

        for (auto n: graph.neighborRange(u))
        {
            g_copy.addEdge(u, n);
        }
    }

    run_ = true;
}


bool isBiconnected(const Graph &g)
{
    NetworKit::BiconnectedComponents generator(g);

    generator.run();

    if (generator.getComponents().size() > 1)
        return false;
    else
        return true;
}

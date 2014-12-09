

#include "storage/Holders/HolderImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/XmlFile.h"


namespace storage_bgl
{

    Holder::Impl::Impl()
	: devicegraph(nullptr)
    {
    }


    Holder::Impl::Impl(const xmlNode* node)
	: devicegraph(nullptr)
    {
    }


    void
    Holder::Impl::set_devicegraph_and_edge(Devicegraph* devicegraph,
					Devicegraph::Impl::edge_descriptor edge)
    {
	Impl::devicegraph = devicegraph;
	Impl::edge = edge;

	const Holder* holder = devicegraph->get_impl().graph[edge].get();
	if (&holder->get_impl() != this)
	    throw runtime_error("wrong edge for back references");
    }


    sid_t
    Holder::Impl::get_source_sid() const
    {
	const Devicegraph::Impl::graph_t& graph = devicegraph->get_impl().graph;
	return graph[source(edge, graph)]->get_sid();
    }


    sid_t
    Holder::Impl::get_target_sid() const
    {
	const Devicegraph::Impl::graph_t& graph = devicegraph->get_impl().graph;
	return graph[target(edge, graph)]->get_sid();
    }


    void
    Holder::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "source-sid", get_source_sid());
	setChildValue(node, "target-sid", get_target_sid());
    }

}

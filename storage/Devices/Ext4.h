#ifndef EXT4_H
#define EXT4_H


#include "storage/Devices/Filesystem.h"


namespace storage_bgl
{

    using namespace std;


    class Ext4 : public Filesystem
    {
    public:

	static Ext4* create(Devicegraph* devicegraph);
	static Ext4* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual string get_displayname() const override { return "ext4"; }

	bool supports_label() const override { return true; }
	unsigned int max_labelsize() const override { return 16; }

	bool supports_uuid() const override { return true; }

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const override { return "Ext4"; }

	virtual Ext4* clone() const override;

    protected:

	Ext4(Impl* impl);

    };

}

#endif

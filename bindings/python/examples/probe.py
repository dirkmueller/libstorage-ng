#!/usr/bin/python

from storage import Environment, Storage


environment = Environment(True)

storage = Storage(environment)

probed = storage.getProbed()

probed.print_graph()


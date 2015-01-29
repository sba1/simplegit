#!/usr/bin/python
#
# This script can be used to transform the revision tree of the git
# repository in the current working directory to a representation
# suitable for a unittest.
#
# (c) 2015 by Sebastian Bauer
#
from subprocess import check_output

# Request from git the revision list with associated parents
out = check_output(["git" , "rev-list", "--parents", "--all"])

lines = out.split("\n")

# Build the index map, this maps commit SHA1s to their index
# in the list
indexmap = {}
index = 0
for line in lines:
	ids = line.split(" ",1)
	if len(ids)>=1:
		indexmap[ids[0]] = index
		index = index + 1


# With the map at hand, we can write out the code
print 'struct node g[] ='
print '{'

for line in lines:
	ids = line.split()
	if len(ids) == 0:
		continue
	mapped_parents = []
	# The parents start at entry 1
	for parent in ids[1:]:
		mapped_parents.append(str(indexmap[parent]))
	print '\t{',
	print len(mapped_parents),
	print ', {',
	print ','.join(mapped_parents),
	print '}',
	print ',\t"Commit ' + str(indexmap[ids[0]]) + '"',
	print '},'

print '};'


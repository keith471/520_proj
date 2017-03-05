Ok so the problem is with lists. I need to break down lists as I parse them into lists of VARDECLARATIONs, etc.
This starts in the parser, and branches out from there. Basically, when I parse things with lists, I need to have
special functions in tree.c that turn the lists into lists of nodes, rather than just one node. I can also catch weeding
errors in the process - i.e. if one thing in one of the lists is null and something in another list isn't, then the
lists are not matching in length and so we have a problem.

So, we need to change the following:

golite.y
tree.c
    tree.h
pretty.c
    pretty.h
weed.c
    weed.h

The pros are that this gives a cleaner solution and makes later phases simpler.

TODOS

[x] fixed variable declarations in golite.y/tree.c
[x] fix parameter declarations in golite.y/tree.c
[x] fix other things that are lists in golite.y/tree.c
    assignment statements, short decl statements, fields
[x] update tree.h
[x] test scanner/parser
[x] update pretty.c
[x] update pretty.h
[x] update main, makefile, and test
[x] update weed.c
[x] update weed.h
[x] test
[ ] modify main to check for errors after parsing!!! (we might have thrown in weeding errors)

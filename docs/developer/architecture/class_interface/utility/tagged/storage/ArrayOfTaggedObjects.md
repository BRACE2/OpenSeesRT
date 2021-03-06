
# ArrayOfTaggedObjects 

```cpp
#include <tagged/storage/ArrayOfTaggedObjects.h>
```

class ArrayOfTaggedObjects\

\

\
ArrayOfTaggedObjects is used as a container object to store and provide
access to objects of type TaggedObject. A single one dimensional array
is used to store the pointers to the objects. As a one dimensional array
is used, certain ideas are tried to improve performance: (1) if the
array needs to be larger to hold more components, the array size is
doubled; (2) when adding/retrieving components, the array location given
by the components tag is first checked; and (3) a boolean flag is used
to keep track of whether all objects have been stored at the location
given by the tags.

### Constructor

\
### Destructor

\
// Pure Public Methods\

\

\

\

\

\

Creates an array of size *sizeInitialArray* to hold pointers to the
TaggedObject objects that will be added to the container. This array is
created using `new()` to allow the array to grow as needed. Zeros this
array and sets a number of integer values: (1) the size of the array;
(2) the last position used in the array is $0$; (3) the position in the
array through which all previous cells are being used for pointers is
$0$; (4) the number of components added to the container is $0$; and (5)
all components have been added at the position given by their tag. If
not enough space is available, the warning method is invoked on the
global ErrorHandler and the integer outlining the array size is set to
$0$.

\
If memory has been allocated for the array, the object invokes the
destructor on the current array.

\
If *newSize* is valid, i.e. $>= 1$ AND *newSize* $>$ current size of the
array, the object allocates memory for a new array of size *newSize*. It
zeros this array and copies the old components to this array, trying to
see if they can be located at the positions given by their tags. If all
placed at ideal location last time and new size $>=$ position of last
entry straight copy, otherwise we reset and add each again using
`addComponent()`. Finally invokes the destructor on the old array.
Returns $0$ if successful. If not successful, the warning method is
invoked on the global ErrorHandler and a negative value is returned.

To add the object *newComponent* to the container. First checks to see
that an object with a similar tag does not already exist in the
container, invokes the warning method on the global ErrorHandler and
returns *false* if one does. It then checks to ensure that array is
large enough to hold another pointer, if not `setSize()` is invoked with
twice the size of the current array. (If `setSize()` fails the warning
method is invoked and *false* is returned: NOTE other objects previously
added may now be missing!!. The object is then placed into the array by
choosing the first of the following that is successful:

1.  If the array is large enough, the location given by the objects tag
    is first tested to see if being used. If not this location is used.

2.  If it won't go in nicely, we find the first location in the array
    that is not being used and use this location. We keep a marker to
    this location for subsequent adds so that don't start at $0$
    location all the time.

Finally the integer indicating the numbers of objects in the array is
incremented and *true* is returned.

To remove the component whose tag is given by *tag* from the container
and return a pointer to the object. If tag is not too large and all
components have been added nicely, the contents of the array at location
*tag* is set to $0$ and its old contents returned. Otherwise if the tag
is not too large, the contents at position *tag* is first checked to see
if it is pointing to an object and if this object has the same tag as
*tag*, if it does the contents of the array is set to $0$ and the object
returned. If the object is not at it's nice location, the array is
searched from the start to the position holding the last entry to see if
the array points to the object with the appropriate tag. If it does the
array location is set to $0$ and the object returned, otherwise $0$ is
returned. If the object has not been found after the last possible
location has been checked, $0$ is returned.

Returns the number of components currently stored in the container.

To return a pointer to the TaggedObject whose identifier is given by
*tag*. If tag is not too large and all components have been added
nicely, the contents of the array at location *tag* is returned.
Otherwise if the tag is not too large, the contents at position *tag* is
first checked to see if it is pointing to an object and if this object
has the same tag as *tag*, the object returned. If the object is not at
it's nice location, the array is searched from the start to the position
holding the last entry to see if the array points to the object with the
appropriate tag. If it does the object is returned. If the object has
not been found after the last possible location has been checked, $0$ is
returned.

To return an iter for iterating through the objects that have been added
to the container. Each container object has its own iter. This iter() is
reset to point to the start and a reference to this iter is returned.

To return an empty copy of the container. Creates a new
ArrayOfTaggedObjects object using the current size of the array as the
argument for the constructor. It is up to the user of this method to
invoke the destructor on the new object.

To remove all objects from the container and **to invoke the destructor
on these objects**. Goes through the array, invoking the destructor on
any object pointed to by a cell and then setting this cell to $0$.
Resets the internal member data to indicate that zero components have
been added to the container.

Invokes `Print(s,flag)`{.cpp} on all objects which have been added to the
container.

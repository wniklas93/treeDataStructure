set(sources
    src/nodes.cpp
)

set(exe_sources
		#src/main.cpp
		${sources}
)

set(headers
    include/tree/nodes.hpp
)

set(test_sources
  src/read_write.cpp
#  src/get_ids.cpp
)

set(experiments_sources
  src/tree.cpp
#  src/memory_footprint.cpp
  )


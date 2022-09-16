set(sources
    src/nodes.cpp
)

set(exe_sources
		#src/main.cpp
		${sources}
)

set(headers
    include/tree/nodes.hpp
    include/tree/nodeFactory.hpp
)

set(test_sources
  src/read.cpp
)

set(experiments_sources
  src/tree.cpp
  )

set(benchmark_sources
  src/memory.cpp
  )

package_add_test(io_gtest io_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(dimacs_reader_gtest dimacs_reader_gtest.cpp
        ../../io/io.cpp
        ../../io/dimacs_reader.cpp
        ../../instance/ising.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(ising_reader_gtest ising_reader_gtest.cpp
        ../../io/ising_reader.cpp
        ../../io/io.cpp
        ../../instance/ising.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )
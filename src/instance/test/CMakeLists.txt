package_add_test(instance_qubo_gtest qubo_gtest.cpp
        ../qubo.cpp
        )

package_add_test(instance_maxcut_gtest maxcut_gtest.cpp
        ../maxcut.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(convert_gtest convert_gtest.cpp
        ../convert.cpp
        ../../io/io.cpp
        ../../instance/qubo.cpp
        ../../instance/maxcut.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(instance_ising_gtest ising_gtest.cpp
        ../ising.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )


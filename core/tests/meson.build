#get gtest
gtest = dependency('gtest', required: true, fallback:['gtest','gtest_dep'])

#make both dependencies into one array
deps = [libtrainsim_core_dep]
deps += gtest

tests = [
    'track_data',
    'train_properties',
    'version',
    'input-axis',
]

#create all the tests
foreach test_obj : tests
    exe = executable(
        test_obj, 
        test_obj + '.cpp',
        dependencies: deps,
        cpp_pch: 'pch/libtrainsim_core_test_pch.hpp',
    )
    test(test_obj + ' Test', exe)
endforeach

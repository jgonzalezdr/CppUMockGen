/**
 * @file
 * @brief      Header of Subtest helper macros
 * @project    CppUMockGen
 * @authors    Jesus Gonzalez <jgonzalez@gdr-sistemas.com>
 * @copyright  Copyright (c) 2022 Jesus Gonzalez. All rights reserved.
 * @license    See LICENSE.txt
 */

#ifndef CppUMockGen_TestHelpers_Subtest_hpp_
#define CppUMockGen_TestHelpers_Subtest_hpp_

#define SUBTEST_BEGIN( x )

#define SUBTEST_END \
    mock().checkExpectations(); \
    mock().clear();

#endif // header guard

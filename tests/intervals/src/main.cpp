//
// Copyright (c) 2019 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofMain.h"
#include "ofAppNoWindow.h"
#include "ofxUnitTests.h"
#include "ofx/IndexRange.h"
#include "ofx/IndexRangeList.h"


class ofApp: public ofxUnitTestsApp
{
    using Range = ofx::IndexRange;
    using RangeList = ofx::IndexRangeList;

    void run() override
    {
        {
            Range range;
            ofxTestEq(range.location, 0, "values");
            ofxTestEq(range.size, 0, "values");
            ofxTestEq(range.getMin(), 0, "values");
            ofxTestEq(range.getMax(), 0, "values");
            ofxTestEq(range.overflows(), false, "overflows");
            ofxTestEq(range.empty(), true, "empty");
        }
        {
            Range range(Range::LOWEST, Range::MAX);
            ofxTestEq(range.location, Range::LOWEST, "values");
            ofxTestEq(range.size, Range::MAX, "values");
            ofxTestEq(range.getMin(), Range::LOWEST, "values");
            ofxTestEq(range.getMax(), Range::LOWEST + Range::MAX, "values");
            ofxTestEq(range.overflows(), false, "overflows");
            ofxTestEq(range.empty(), false, "empty");
        }
        {
            Range range(1, Range::MAX);
            ofxTestEq(range.location, 1, "values");
            ofxTestEq(range.size, Range::MAX, "values");
            ofxTestEq(range.getMin(), 1, "values");
            ofxTestEq(range.getMax(), 0, "values");
            ofxTestEq(range.overflows(), true, "overflows");
            ofxTestEq(range.empty(), false, "empty");
        }
        {
            std::vector<std::pair<std::size_t, std::size_t>> minmax =
            {
                { 0, 0 },
                { 0, 1 },
                { 0, Range::MAX },
                { Range::MAX, Range::MAX },
            };

            Range range(0, 0);

            for (auto& mm: minmax)
            {
                range.setMin(mm.first);
                ofxTestEq(range.getMin(), mm.first, "getter/setter");
                range.setMax(mm.second);
                ofxTestEq(range.getMax(), mm.second, "getter/setter");
            }

            for (auto& mm: minmax)
            {
                range.setMax(mm.second);
                ofxTestEq(range.getMax(), mm.second, "getter/setter");
                range.setMin(mm.first);
                ofxTestEq(range.getMin(), mm.first, "getter/setter");
            }

            range.setMin(Range::MAX);
            range.setMax(Range::MAX);
            ofxTestEq(range.getMax(), Range::MAX, "getter/setter");
            ofxTestEq(range.getMin(), Range::MAX, "getter/setter");

            range.setMax(0);
            ofxTestEq(range.getMax(), 0, "getter/setter");
            ofxTestEq(range.getMin(), 0, "getter/setter");

            range.setMin(Range::MAX);
            ofxTestEq(range.getMax(), Range::MAX, "getter/setter");
            ofxTestEq(range.getMin(), Range::MAX, "getter/setter");
        }
        {

            typedef Range Input;
            typedef std::tuple<bool, Range, Range> Results;
            typedef std::pair<Input, Results> Data;

            std::vector<Data> data =
            {
                { { 0, 0 }, { false, { 0, 0 }, { 0, 0 } } },
                { { 0, 1 }, { false, { 0, 0 }, { 0, 1 } } },
                { { 0, Range::MAX }, { false, { 0, 0 }, { 0, Range::MAX } } },
                { { 1, Range::MAX }, { true,  { 0, 1 }, { 1, Range::MAX - 1 } } },
                { { 2, Range::MAX }, { true,  { 0, 2 }, { 2, Range::MAX - 2 } } },
                { { 3, Range::MAX }, { true,  { 0, 3 }, { 3, Range::MAX - 3 } } },
                { { Range::MAX, Range::MAX }, { true,  { 0, Range::MAX }, { Range::MAX, Range::MAX - Range::MAX } } }
            };

            for (auto& datum: data)
            {
                Range original = datum.first;

                bool shouldOverflow = std::get<0>(datum.second);
                Range input = datum.first;
                Range remainder = std::get<1>(datum.second);
                Range inputResult = std::get<2>(datum.second);

                ofxTest(original.overflows() == shouldOverflow, "normalize - overflows");
                ofxTest(input.clearOverflow() == remainder, "normalize - normalize");
                ofxTest(input == inputResult, "normalize - check");
                ofxTest(remainder.location == 0, "normalize - continuity");
                ofxTest(remainder.size + inputResult.size == original.size, "normalize - continuity");
                ofxTest(remainder.overflows() == false, "normalize - overflows");
                ofxTest(inputResult.overflows() == false, "normalize - overflows");
            }
        }
        {
            ofxTest(Range(0, 0) == Range(0, 0), "==");

            ofxTest(Range(0, 0) < Range(1, 0), "<");
            ofxTest(Range(1, 0) < Range(1, 1), "<");
            ofxTest(Range(1, 1) < Range(2, 1), "<");
            ofxTest(Range(2, 1) < Range(2, 2), "<");
            ofxTest(Range(Range::MAX, 1) < Range(Range::MAX, 2), "<");

            // All other comparison operators are in terms of == and <.
        }
        {
            ofxTestEq(Range::fromInterval(0, 0), Range::fromInterval(0, 0), "fromInterval");
            ofxTestEq(Range::fromInterval(0, 1), Range::fromInterval(1, 0), "fromInterval - ordered");
            ofxTestEq(Range::fromInterval(0, 0), Range(0, 1), "fromInterval");
            ofxTestEq(Range::fromInterval(0, Range::MAX - 1), Range(0, Range::MAX), "fromInterval - extrema");
        }
        {
            Range a(0, 1);
            std::stringstream ss;
            ss << a;
            Range v;
            ss >> v;

            ofxTestEq(a, v, "IO");
            ofxTestEq(ofJson(a), a, "Json");
        }
        {
            ofxTest(Range(0, 100).contains(0), "contains");
            ofxTest(Range(0, 100).contains(99), "contains");
            ofxTest(Range(0, Range::MAX).contains(Range::MAX - 1), "contains");
            ofxTest(Range(0, 1).contains(0), "contains");
            ofxTest(Range(1, 1).contains(1), "contains");

            ofxTest(!Range(0, 0).contains(0), "contains");
            ofxTest(!Range(0, 100).contains(100), "contains");
            ofxTest(!Range(0, Range::MAX).contains(Range::MAX), "contains");
        }
        {
            ofxTest(Range(0, 100).contains(Range(0, 99)), "contains");
            ofxTest(Range(0, 100).contains(Range(0, 100)), "contains");
            ofxTest(!Range(0, 100).contains(Range(0, 101)), "contains");
            ofxTest(!Range(1, 100).contains(Range(0, 101)), "contains");
            ofxTest(!Range(0, Range::MAX).contains(Range(0, 0)), "contains");
            ofxTest(Range(0, Range::MAX).contains(Range(0, 1)), "contains");
        }
        {
            ofxTest(Range(1, 1).isHighAdjacentTo(Range(0, 1)), "isHighAdjacentTo");
            ofxTest(!Range(0, 1).isHighAdjacentTo(Range(0, 1)), "isHighAdjacentTo");
            ofxTest(!Range(0, 1).isHighAdjacentTo(Range(2, 1)), "isHighAdjacentTo");
            ofxTest(Range(Range::MAX, 0).isHighAdjacentTo(Range(Range::MAX, 0)), "isHighAdjacentTo");

            ofxTest(Range(1, 1).isLowAdjacentTo(Range(2, 1)), "isLowAdjacentTo");
            ofxTest(!Range(1, 1).isLowAdjacentTo(Range(3, 1)), "isLowAdjacentTo");
            ofxTest(!Range(0, 1).isLowAdjacentTo(Range(0, 1)), "isLowAdjacentTo");
            ofxTest(Range(Range::MAX, 0).isLowAdjacentTo(Range(Range::MAX, 0)), "isLowAdjacentTo");
        }
        {
            ofxTest(Range(0, 1).intersects(Range(0, 1)), "intersects");
            ofxTest(!Range(0, 0).intersects(Range(0, 1)), "intersects");
            ofxTest(!Range(100, 100).intersects(Range(0, 100)), "intersects");
            ofxTest(Range(100, 10).intersects(Range(0, 101)), "intersects");
            ofxTest(Range(Range::MAX - 1, 1).intersects(Range(0, Range::MAX)), "intersects");
            ofxTest(!Range(Range::MAX, 2).intersects(Range(0, Range::MAX)), "intersects");
            ofxTest(!Range(Range::MAX, 1).intersects(Range(0, Range::MAX)), "intersects");
            ofxTest(Range(1, Range::MAX).intersects(Range(0, Range::MAX)), "intersects");

        }
        {
            ofxTest(Range(0, 1).intersectionWith(Range(0, 1)) == Range(0, 1), "intersectionWith");
            ofxTest(Range(0, 0).intersectionWith(Range(0, 1)) == Range(), "intersectionWith");
            ofxTest(Range(100, 100).intersectionWith(Range(0, 100)) == Range(), "intersectionWith");
            ofxTest(Range(100, 10).intersectionWith(Range(0, 101)) == Range(100, 1), "intersectionWith");
            ofxTest(Range(Range::MAX - 1, 1).intersectionWith(Range(0, Range::MAX)) == Range(Range::MAX - 1, 1), "intersectionWith");
            ofxTest(Range(Range::MAX, 1).intersectionWith(Range(0, Range::MAX)) == Range(), "intersectionWith"); //
            ofxTest(Range(1, Range::MAX).intersectionWith(Range(0, Range::MAX)) == Range(1, Range::MAX), "intersectionWith");
            ofxTest(Range(1, Range::MAX).intersectionWith(Range(999, Range::MAX)) == Range(), "intersectionWith");

        }
        {
            ofxTest(Range(0, 1).unionWith(Range(0, 1)) == Range(0, 1), "unionWith");
            ofxTest(Range(0, 0).unionWith(Range(0, 1)) == Range(0, 1), "unionWith");
            ofxTest(Range(100, 100).unionWith(Range(0, 100)) == Range(0, 200), "unionWith");
            ofxTest(Range(100, 10).unionWith(Range(0, 101)) == Range(0, 110), "unionWith");

            ofxTest(Range(Range::MAX - 1, 1).unionWith(Range(0, Range::MAX)) == Range(0, Range::MAX), "unionWith");
            ofxTest(Range(Range::MAX, 1).unionWith(Range(0, Range::MAX)) == Range(0, Range::MAX), "unionWith");

            ofxTest(Range(0, 1).unionWith(Range(Range::MAX, Range::MAX)) == Range(0, Range::MAX - 1), "unionWith");
        }
        {
            ofxTest(Range().mergeWith(Range()) == Range(), "mergeWith");
            ofxTest(Range().mergeWith(Range(0, 1)) == Range(0, 1), "mergeWith");
            ofxTest(Range(1, 1).mergeWith(Range(0, 1)) == Range(0, 2), "mergeWith");
            ofxTest(Range(0, 1).mergeWith(Range(0, 1)) == Range(0, 1), "mergeWith");
            ofxTest(Range(0, 1).mergeWith(Range(2, 1)) == Range(    ), "mergeWith"); // No intersect or Adjacent.
            ofxTest(Range(0, 1).mergeWith(Range(1, 1)) == Range(0, 2), "mergeWith");
            ofxTest(Range(Range::MAX, 0).mergeWith(Range(Range::MAX, 0)) == Range(Range::MAX, 0), "mergeWith"); // Both invalid.
            ofxTest(Range(1, 1).mergeWith(Range(2, 1)) == Range(1, 2), "mergeWith");
            ofxTest(Range(1, 1).mergeWith(Range(3, 1)) == Range(), "mergeWith");
            ofxTest(Range(100, 100).mergeWith(Range(0, 100)) == Range(0, 200), "mergeWith");
            ofxTest(Range(100, 10).mergeWith(Range(0, 101)) == Range(0, 110), "mergeWith");
            ofxTest(Range(Range::MAX - 1, 1).mergeWith(Range(0, Range::MAX)) == Range(0, Range::MAX), "mergeWith");
            ofxTest(Range(Range::MAX, 1).mergeWith(Range(0, Range::MAX)) == Range(0, Range::MAX), "mergeWith");
            ofxTest(Range(Range::MAX, 0).mergeWith(Range(0, Range::MAX)) == Range(0, Range::MAX), "mergeWith");
        }

        {
            std::vector<Range> ranges = {
                Range::fromInterval(1, 1),
                Range::fromInterval(1, 2),
                Range::fromInterval(4, 10),
                Range::fromInterval(5, 6),
                Range::fromInterval(11, 11)
            };

            std::vector<Range> merged = {
                Range::fromInterval(1, 2),
                Range::fromInterval(4, 11)
            };

            RangeList list(ranges);

            ofxTestEq(list.ranges().size(), merged.size(), "RangeList::size()");
            for (std::size_t i = 0; i < merged.size(); ++i)
                ofxTestEq(list.ranges()[i], merged[i], "RangeList");

            // Invariant under random shuffling.

            std::random_shuffle(ranges.begin(), ranges.end());

            list = RangeList(ranges);

            ofxTestEq(list.ranges().size(), merged.size(), "RangeList::size()");
            for (std::size_t i = 0; i < merged.size(); ++i)
                ofxTestEq(list.ranges()[i], merged[i], "RangeList");


            std::vector<Range> insertedA = {
                Range::fromInterval(
                    1 + 0 + 0 + 0 + 100,
                    2 + 3 + 0 + 0 + 100
                ),
                Range::fromInterval(
                    4 + 3 + 0 + 0 + 100,
                    11+ 3 + 1 + 0 + 100
                ),
            };

            list.insert(Range(1, 3));
            list.insert(Range(15, 100)); // No effect.
            list.insert(Range(14, 1));
            list.insert(Range(17, 1)); // No effect.
            list.insert(Range(0, 100));

            ofxTestEq(list.ranges().size(), insertedA.size(), "RangeList::size()");
            for (std::size_t i = 0; i < insertedA.size(); ++i)
                ofxTestEq(list.ranges()[i], insertedA[i], "RangeList");

            // Push everything off the high end.
            list.insert(Range(0, Range::MAX));

            ofxTestEq(list.size(), 0, "RangeList");

            insertedA = { };

            list.add(Range(0, Range::MAX));
            insertedA = { Range(0, Range::MAX) };

            ofxTestEq(list.ranges().size(), insertedA.size(), "RangeList::size()");
            for (std::size_t i = 0; i < insertedA.size(); ++i)
                ofxTestEq(list.ranges()[i], insertedA[i], "RangeList");
        }


        {
            {
                std::vector<Range> ranges = {
                    { 100, 100 },
                    Range::fromExclusiveInterval(600, Range::MAX)
                };

                {
                    std::vector<Range> results = {
                         { 100, 100 },
                        Range::fromExclusiveInterval(600, Range::MAX)
                    };

                    RangeList list(ranges);

//                    // Below
//                    list.remove({ 0, 100 });
//
//                    // Above
//                    list.remove({ 200, 100 });
//
//
//                    // Exact
//                    list.remove({ 100, 100 });
//                    list.add({ 100, 100 });
//
//                    // Completely covers.
//                    list.remove({ 0, Range::MAX });
//                    list.add({ 100, 100 });

//                    // Overlap low.
//                    list.remove({ 50, 100 });
//                    results[0] = { 150, 50 };

                    // Overlap middle.
                    list.remove({ 125, 50 });
                    list.remove({ 650, 50 });
                    results = {
                        { 100, 25 },
                        { 175, 25 },
                        Range::fromExclusiveInterval(600, 650),
                        Range::fromExclusiveInterval(700, Range::MAX)
                    };

                    //                    // Overlap high.
//                    list.remove({ 150, 100 });


                    ofxTestEq(list.ranges().size(), results.size(), "RangeList::size()");
                    for (std::size_t i = 0; i < results.size(); ++i)
                        ofxTestEq(list.ranges()[i], results[i], "RangeList");

                }
            }
        }


        auto testErase = [&](const std::vector<Range>& iter,
                             const std::vector<Range>& toErase,
                             const std::vector<Range>& results) -> void
        {
            RangeList list(iter);
            for (auto& range: toErase)
                list.erase(range);
            ofxTestEq(list.ranges().size(), results.size(), "RangeList::size()");
            for (std::size_t i = 0; i < results.size(); ++i)
                ofxTestEq(list.ranges()[i], results[i], "RangeList");
        };


        {
            // [  iter  ] // empty
            // [  range ] // empty
            testErase({ { 0, 0 } },
                      { { 0, 0 } },
                      { });

        }

        {
            // [  iter  ] // empty
            // [  range ] // non-empty
            testErase({ { 0, 0 } },
                      { { 0, 1 } },
                      { });
        }
        {
            // [  iter  ] // non-empty
            // [  range ] // empty
            testErase({ { 0, 1 } },
                      { { 0, 0 } },
                      { { 0, 1 } });
        }

        {
            // [  iter  ] // invalid
            // [  range ] // invalid
            testErase({ { Range::MAX, Range::MAX } },
                      { { Range::MAX, Range::MAX } },
                      { });
        }

        {
            // [  iter  ] // invalid
            // [  range ] // valid
            testErase({ { Range::MAX, Range::MAX } },
                      { { 0, 1 } },
                      { });
        }

        {
            // [  iter  ] // valid
            // [  range ] // invalid
            testErase({ { 0, 1 } },
                      { { Range::MAX, Range::MAX } },
                      { { 0, 1 }});
        }

        {
            // [  iter       ] // max
            // [  range ] // 0 - valid
            testErase({ { 0, Range::MAX } },
                      { { 0, 1 } },
                      { { 0, Range::MAX - 1 }});
        }

        {
            // [  iter       ] // 0 - valid
            // [  range ] // 0 - valid
            testErase({ { 0, 100 } },
                      { { 0, 1 } },
                      { { 0, 100 - 1 }});
        }


        {
            // [  iter  ] // 0 - valid
            // [  range ] // max
            testErase({ { 0, 1 } },
                      { { 0, Range::MAX } },
                      { });
        }
        {
            // [  iter  ]
            // [  range ]
            testErase({ { 0, 1 } },
                      { { 0, 1 } },
                      { });
        }

        {
            // [  iter  ]
            // [  range ]
            testErase({ { 0, 1 } },
                      { { 0, Range::MAX + 2 } }, // This overflows to 0, 1
                      { });

        }
        {
            // [  iter  ]
            //                   [  range ]
            testErase({ { 0, 100 } },
                      { { 200, 100 } },
                      { { 0, 100 } });

        }

        {
            // [  iter  ]
            //           [  range ]
            testErase({ { 100, 100 } },
                      { { 200, 100 } },
                      { { 100, 100 } });
        }

        {
            // [  iter  ]
            //       [  range ]
            testErase({ { 100, 150 } },
                      { { 200, 100 } },
                      { { 100, 100 } });
        }
        {
            // [        iter       ]
            //       [  range ]
            testErase({ { 100, 350 } },
                      { { 200, 100 } },
                      { { 100, 250 } });
        }

        {
            //     [ iter ]
            // [     range     ]
            testErase({ { 250, 25 } },
                      { { 200, 100 } },
                      { });
        }

        {
            // [      iter    ]
            // [  range ]
            testErase({ { 200, 300 } },
                      { { 200, 100 } },
                      { { 200, 200 } });
        }

        {
            //    [ iter ]
            // [  range  ]
            testErase({ { 250, 50 } },
                      { { 200, 100 } },
                      { });
        }

        {
            //     [ iter   ]
            // [  range ]
            testErase({ { 250, 100 } },
                      { { 200, 100 } },
                      { { 200, 50 } });
        }
        {
            //          [ iter   ]
            // [  range ]
            testErase({ { 300, 100 } },
                      { { 200, 100 } },
                      { { 200, 100 } });
        }

        {
            //              [ iter   ]
            // [  range ]
            testErase({ { 500, 100 } },
                      { { 200, 100 } },
                      { { 400, 100 } });
        }

    }

};


int main()
{
    ofInit();
    auto window = make_shared<ofAppNoWindow>();
    auto app = make_shared<ofApp>();
    ofRunApp(window, app);
    return ofRunMainLoop();
}

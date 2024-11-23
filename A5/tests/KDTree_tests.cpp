#include <gtest/gtest.h>

#include <unordered_map>

#include "KDTree.hpp"

struct vec2i
{
    int v1;
    int v2;
};

bool operator==(const vec2i & vec1, const vec2i & vec2)
{
    return (vec1.v1 == vec2.v1) && (vec1.v2 == vec2.v2);
}

struct vec2i_compare1
{
    bool operator()(const vec2i & first, const vec2i & second)
    {
        return first.v1 < second.v1;
    }
};

struct vec2i_compare2
{
    bool operator()(const vec2i & first, const vec2i & second)
    {
        return first.v2 < second.v2;
    }
};

struct vec2i_distance1
{
    int operator()(const vec2i & first, const vec2i & second)
    {
        return abs(first.v1 - second.v1);
    }
};

struct vec2i_distance2
{
    int operator()(const vec2i & first, const vec2i & second)
    {
        return abs(first.v2 - second.v2);
    }
};

struct Distance
{
    int operator()(const int & first, const int & second)
    {
        return abs(first - second);
    }
};

template <typename T>
bool AreVectorsEqualIgnoringOrder(const std::vector<T>& vec1, const std::vector<T>& vec2) {
    if (vec1.size() != vec2.size()) {
        return false;
    }

    // Use unordered_map to count occurrences of each item in the vectors
    std::unordered_map<T, int> count_map1, count_map2;

    // Count occurrences in the first vector
    for (const auto& item : vec1) {
        count_map1[item]++;
    }

    // Count occurrences in the second vector
    for (const auto& item : vec2) {
        count_map2[item]++;
    }

    // Compare the two maps
    return count_map1 == count_map2;
}

TEST(KDTreeTests, Tree1DGetNearest)
{
    std::vector<int> ints{5, 6, 7, 8, 9, 10, 1, 2, 3};

    KDTree<int, int> tree(ints, {std::less_equal<int>()}, {Distance()});

    EXPECT_EQ(tree.getNearest(5), 5);
    EXPECT_EQ(tree.getNearest(7), 7);
    EXPECT_EQ(tree.getNearest(10), 10);

    EXPECT_EQ(tree.getNearest(12), 10);
}

TEST(KDTreeTests, Tree1DGetKNearest)
{
    std::vector<int> ints{5, 6, 7, 8, 9, 10, 1, 2, 3};

    KDTree<int, int> tree(ints, {std::less_equal<int>()}, {Distance()});

    EXPECT_TRUE(AreVectorsEqualIgnoringOrder(tree.getKNearest(4, 2), {3, 5}));
    EXPECT_TRUE(AreVectorsEqualIgnoringOrder(tree.getKNearest(11, 3), {10, 9, 8}));
}

TEST(KDTreeTests, Tree2DDegenerate)
{
    std::vector<vec2i> vec{{0, 0}, {0, 2}, {0, 3}, {0, 2}, {0, 1}};

    KDTree<vec2i, int> tree(vec, {vec2i_compare1(), vec2i_compare2()}, {vec2i_distance1(), vec2i_distance2()});

    EXPECT_EQ(tree.getNearest({0, 0}), (vec2i{0, 0}));
    EXPECT_EQ(tree.getNearest({0, 1}), (vec2i{0, 1}));
    EXPECT_EQ(tree.getNearest({0, 2}), (vec2i{0, 2}));
    EXPECT_EQ(tree.getNearest({0, 3}), (vec2i{0, 3}));
}

TEST(KDTreeTests, Tree2DGetNearest) {
    std::vector<vec2i> vec{{0, 0}, {1, 3}, {3, 3}, {-2, -2}, {-9, -9}};

    KDTree<vec2i, int> tree(vec, {vec2i_compare1(), vec2i_compare2()}, {vec2i_distance1(), vec2i_distance2()});

    EXPECT_EQ(tree.getNearest({0, 0}), (vec2i{0, 0}));
    EXPECT_EQ(tree.getNearest({1, 1}), (vec2i{0, 0}));
    EXPECT_EQ(tree.getNearest({-3, -3}), (vec2i{-2, -2}));
    EXPECT_EQ(tree.getNearest({-10, -10}), (vec2i{-9, -9}));
}

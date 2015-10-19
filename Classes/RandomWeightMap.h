#include <map>
#include <assert.h>
#include <sstream>


//basically a dictionary that'll let you add an item with a specific weighted
//score, so I can simplify drops and stuff
template<typename T>
class RandomWeightMap
{
    private:
        std::map<T, int> _map;

    public:
        void add_item(T item, int weight);
        T get_item(int score);
        T get_item();

        int get_total_weight();
        RandomWeightMap();
};
    template<typename T>
RandomWeightMap<T>::RandomWeightMap()
{
    this->_map = std::map<T, int>();
};

    template<typename T>
void RandomWeightMap<T>::add_item(T item, int weight)
{
    std::pair<T, int> pair = std::pair<T, int>(item, weight);
    this->_map.insert(pair);
};

    template<typename T>
T RandomWeightMap<T>::get_item()
{
    int total_weight = this->get_total_weight();
    // printf("total weight %d\n", total_weight);
    int score = rand() % total_weight;
    return this->get_item(score);
};

    template<typename T>
T RandomWeightMap<T>::get_item(int score)
{
    int total_weight = this->get_total_weight();
    int cumulative_weight = 0;
    std::cout << " " << score << " score" << std::endl;
    for (auto it = this->_map.begin(); it != this->_map.end(); it++)
    {
        cumulative_weight+=it->second;
        if (score <= cumulative_weight)
        {
            return it->first;
        };
    };

    std::stringstream ss;
    ss << "score: " << score << " out of range: " << total_weight;
    assert(false && ss.str().c_str());
    return this->_map.begin()->first;
};

    template<typename T>
int RandomWeightMap<T>::get_total_weight()
{
    int total_weight = 0;
    for (auto it = this->_map.begin(); it != this->_map.end(); it++)
    {
        total_weight+=it->second;
    };
    return total_weight;
};

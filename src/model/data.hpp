#ifndef DATA_HPP
#define DATA_HPP

enum Data {
    Images       = 1,
    ObjectModels = 2,
    Poses        = 4
};

inline Data operator|(Data a, Data b)
{
    return static_cast<Data>(static_cast<int>(a) | static_cast<int>(b));
}


#endif // DATA_HPP

# multimap

1. 可以通过key排序，可以是小数，需要显示更多小数位使用std::setprecision(9)
2. 插入map中的数据是拷贝,
3. std::greater<double> 大到小  std::less<double> 小到大
4. 当指针指向multimap的begin val时 erase首个元素 通过指针获取的内容一定了
- 例子:
``` c++
#include <map>
#include <string>
#include <iostream>
#include <utility>
#include <iomanip>

template <class Map>
void orderDisplay(const Map& m)
{
    for (typename Map::const_iterator i = m.begin(), e = m.end(); i != e; ++i)
        std::cout << '(' << std::setprecision(9)<< i->first << ", " << i->second.dPrice <<","<< i->second.pOrder_id << ") ";
    std::cout << '\n';
}

typedef struct TAGTORDER
{
    std::string pOrder_id;      //订单id
    std::string pUser_id;       //用户id
    int nOrder_type;            //订单类型（市价、限价）
    int nSide;                  //买卖类型
    int nExpire_time;           //过期时间
    double dPrice;              //委托价(市单价该字段无效)
    double dTotal;              //委托量(限价都是委托数量， 市价买卖分开)

    TAGTORDER()
    {
        memset(this, 0, sizeof(*this));
    }
} ORDER, *PORDER;


int main()
{
    
    // typedef std::multimap<double, int, std::greater<double> > Map;
    typedef std::multimap<double, ORDER, std::less<double> > Map;
    typedef std::pair<double, ORDER> value_type;
    Map m;
    PORDER tTakeData = new ORDER;
    tTakeData->dPrice = 10.01;
    tTakeData->pOrder_id = "123456";
    tTakeData->nExpire_time = 1245;
    PORDER tTakeData1 = new ORDER;
    tTakeData1->dPrice = 10.011;
    tTakeData1->pOrder_id = "123456";
    tTakeData1->nExpire_time = 12453;
    m.insert(value_type(tTakeData->dPrice, *tTakeData));
    m.insert(value_type(tTakeData1->dPrice, *tTakeData1));

    tTakeData->dPrice = 8.01;
    tTakeData->pOrder_id = "96547";
    tTakeData->nExpire_time = 23;

    tTakeData1->dPrice = 1.011;
    tTakeData1->pOrder_id = "45678";
    tTakeData1->nExpire_time = 43;
    orderDisplay(m);
}

```
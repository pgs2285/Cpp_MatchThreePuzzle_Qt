#include "board.h"
#include "Consts.h"
#include "item.h"
#include "QDebug"

Board::Board(QGraphicsScene* scene)
    : _scene(scene), _gen(_rand())
{
    _scene -> addItem(&_root);
    _root.setX(_scene->sceneRect().width() / 2 - (Consts::BOARD_SIZE / 2 * Consts::ITEM_SIZE) );
    _root.setY(_scene->sceneRect().height() / 2 - (Consts::BOARD_SIZE / 2 * Consts::ITEM_SIZE) );
    for(int row = 0; row < Consts::BOARD_SIZE; ++row)
    {
        std::vector<Item*> _row(Consts::BOARD_SIZE); // 보드의 사이즈를 미리 지정해둔다.
        _items.push_back(_row);
        for(int column = 0; column < Consts::BOARD_SIZE; ++column)
        {
            addItem(row,column);
        }
    }
}

void Board::addItem(int row, int column) // column에 이미지를 넣어주기 위함.
{

    // device 와 gen은 한번만 뽑는게 (시드) 나을거같으니 생성자로 빼준다.
    std::uniform_int_distribution<int> randomNum(0, Consts::img_count - 1);
    const std::string& path = Consts::img_path[randomNum(_gen)];
   /* QPixmap pixmap(Consts::img_path[randomNum(_gen)].c_str()); // low string 즉 /c등을 그냥 문자그대로 인식하게 R"()"을사용
    // QPixmap scaled(pixmap.scaled(Consts::ITEM_SIZE, Consts::ITEM_SIZE));
    pixmap = pixmap.scaled(Consts::ITEM_SIZE, Consts::ITEM_SIZE);   // 대입. return값이 QPixmap이므로 대입해주자 (유니티 생각하고 자동으로 조정될 생각 ㄴ)
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pixmap, &_root); */  // pixmap에서 로드한 img를 띄워주는 역할을 한다.
    // 그냥 GraphicsPixmapItem item = pixmap 할수도 있지만 지역변수라 여기를 벗어나면 모두 사라지기 때문에 new로 생성해주자.

    Item* item = new Item(this, path, row, column, &_root);
    item->setPos(column * Consts::ITEM_SIZE , row * Consts::ITEM_SIZE);


    _items[row][column] = item;        // push_back 안하고 이렇게 가능한건 vector의 크기를 지정해서 그럼.
                                        // 추후 재활용 할거니 일단 이렇게 적어두자.
}

Board::~Board()
{
    for(int row = 0; row < Consts::BOARD_SIZE; row++)
    {
        for (int column = 0; column < Consts::BOARD_SIZE; column++)
        {
            removeItem(row,column);
        }
    }
}

void Board::itemDragEvent(Item* item, Item::Direction dir)
{
//    qDebug() << item << " : " << static_cast<int>(dir) ;
    int column0 = item->getColumn();
    int row0 = item->getRow();
    int column1 = column0;
    int row1 = row0;

    switch(dir)
    {
    case Item::Direction::Left:
        column1--;
        break;
    case Item::Direction::Right:
        column1++;
        break;
    case Item::Direction::Up:
        row1--;
        break;
    case Item::Direction::Down:
        row1++;
        break;
    }
    if(row1 < 0 || column1 < 0) return;
    if(row1 >= Consts::BOARD_SIZE || column1 >= Consts::BOARD_SIZE) return;
    Item* item0 = item;
    Item* item1 = _items[row1][column1];
    if(item1 == nullptr || item0 == nullptr)
        return;
    exchangeItems(row0, row1, column0, column1);
}
void Board::exchangeItems(int row0, int row1, int column0, int column1){
    Item* item0 = _items[row0][column0];
    Item* item1 = _items[row1][column1];

    moveItem(item1, row0, column0);
    moveItem(item0, row1, column1);

}
void Board::moveItem(Item* item, int row, int column)
{
    item->setColumn(column);
    item->setRow(row);
    item->setPos(column * Consts::ITEM_SIZE , row * Consts::ITEM_SIZE);     // 실제 화면에 보이는 값을 바꿔준다
    _items[row][column] = item;                                             // 관리하고 있는 2차원 vector값을 바꿔준다.
}

void Board::removeItem(int row, int column){
    auto* item = _items[row][column];
    if(item == nullptr) return;
    _items[row][column] = nullptr;
    item -> setParentItem(nullptr);     // nullptr로 부모님을 뺴주고,
    _scene->removeItem(item);           // 아이템 제거후
    delete item;                        // 메모리에서 해제해줌.
}

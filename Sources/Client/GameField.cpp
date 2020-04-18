#include "GameField.h"
#include <QPainter>

/**
 * Конструктор
 * @param cellSize Размер ячейки
 * @param fieldSize Размер поля (ячеек по ширине и высоте)
 */
GameField::GameField(qreal cellSize, const QPoint &fieldSize):cellSize_(cellSize),fieldSize_(fieldSize){
    this->addShip({0,0},Ship::VERTICAL,3,false);
    this->addShip({2,2},Ship::HORIZONTAL,2,true);
}

/**
 * Деструктор
 */
GameField::~GameField() {
    qDeleteAll(shipParts_);
    qDeleteAll(ships_);
}


/**
 * Описывающий прямоугольник
 * @return Прямоугольник
 */
QRectF GameField::boundingRect() const {
    return {QPointF(0.0f,0.0f),QPointF(cellSize_ * 11.0f, cellSize_ * 11.0f)};
}

/**
 * Отрисовка игрового поля со всеми кораблями и прочими объектами
 * @param painter Объект painter
 * @param option Параметры рисования
 * @param widget Указатель на виджет, на котором просиходит отрисовка
 */
void GameField::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // Заличка не нужна
    painter->setBrush(Qt::NoBrush);
    // Черные линии толщиной в 1 пиксель
    painter->setPen(QPen(QColor(0,0,0),1.0f,Qt::PenStyle::SolidLine,Qt::PenCapStyle::SquareCap,Qt::PenJoinStyle::MiterJoin));

    // Символы первого ряда
    //TODO: придумать что-то на случаей если поле больше чем 10 клеток в ширину
    wchar_t symbols[10] = {u'Р',u'Е',u'С',u'П',u'У',u'Б',u'Л',u'И',u'К',u'А'};

    // Нарисовать поле
    for(int i = 0; i < fieldSize_.x()+1; i++)
    {
        for(int j = 0; j < fieldSize_.y()+1; j++)
        {
            // Номера
            if(j == 0 && i > 0){
                painter->drawText(QRectF(j * cellSize_, i * cellSize_, cellSize_, cellSize_), QString::number(i), QTextOption(Qt::AlignCenter));
            }

            // Буквы
            if(i == 0 && j > 0){
                painter->drawText(QRectF(j * cellSize_, i * cellSize_, cellSize_, cellSize_), QString(symbols[j - 1]), QTextOption(Qt::AlignCenter));
            }

            // Клетки
            if(j > 0 && i > 0){
                // Нарисовать клетку
                painter->drawRect(static_cast<int>(j * cellSize_), static_cast<int>(i * cellSize_), cellSize_, cellSize_);
            }
        }
    }

    // Рисование частей кораблей
    for(auto& part : shipParts_)
    {
        // Если часть не принадлежит кораблю, любо корабль есть и он НЕ фантомный
        if(part->ship == nullptr || (part->ship != nullptr && !(part->ship->isPhantom))){
            // Заливка не нужна
            painter->setBrush(Qt::NoBrush);
            // Перо (синие линии)
            painter->setPen(QPen(
                    QColor(0,0,255),
                    4.0f,
                    Qt::PenStyle::SolidLine,
                    Qt::PenCapStyle::SquareCap,
                    Qt::PenJoinStyle::MiterJoin));
            // Рисование части
            part->draw(painter,shipParts_,cellSize_);
        }
        // Если это фантомный корабль (для обозначения)
        else{
            // Отключить карандаш
            painter->setPen(Qt::NoPen);
            // Заливка (цвет меняется в зависимости от нарушения правил размещения)
            painter->setBrush(!part->ship->placementRulesViolated ? QBrush({0,255,0,100}) : QBrush({255,0,0,100}));
            // Рисование части
            painter->drawRect(
                    static_cast<int>((part->position.x() + 1) * cellSize_),
                    static_cast<int>((part->position.y() + 1) * cellSize_),
                    cellSize_,
                    cellSize_);
        }
    }
}

/// I N T E R A C T I O N

/**
 * Добавление корабля
 * @param position Положение начальной ячейки (части) корабля
 * @param orientation Ориентация (относительно начальной ячейки)
 * @param shipLength Длина
 * @param isPhantomShip Фантомный корабль (игнорирует правила размещения, но факт нарушения правил фиксируется)
 * @return Удалось ли добавить корабль на поле
 */
bool GameField::addShip(const QPoint &position, Ship::Orientation orientation, int shipLength, bool isPhantomShip) {

    // Область игрового поля
    QRect gameFieldRect(0,0,fieldSize_.x(),fieldSize_.y());

    // Создать корабль
    auto ship = new Ship;
    ship->orientation = orientation;
    ship->placementRulesViolated = false;
    ship->isPhantom = isPhantomShip;

    // Построить части корябля и проверить каждую часть возможность ее добавления на поле
    for(int i = 0; i < shipLength; i++)
    {
        // Положение новой части на поле
        QPoint partPosition = (ship->orientation == Ship::HORIZONTAL) ? position + QPoint(i,0) : position + QPoint(0,i);

        // Если часть не за пределами поля
        if(gameFieldRect.contains(partPosition))
        {
            // Создать часть корабля
            auto part = new ShipPart;
            part->ship = ship;
            part->position = partPosition;
            part->isBeginning = i == 0;
            part->isDestroyed = false;

            // Может ли ячейка быть зазмещена по правилам
            bool partCanBePlaced = part->canBePlaced(shipParts_);

            // Если ячейка не может быть размещена - значит правила размещения корабля были нарушены
            if(!partCanBePlaced) ship->placementRulesViolated = true;

            // Если правила не были нарушены, либо если это фантомный корабль - добавить ячейку
            if(!ship->placementRulesViolated || isPhantomShip)
            {
                // Добавить в корабль
                ship->parts.push_back(part);
                // Добавить на поле
                shipParts_.push_back(part);
            }
            // Если ячейка не должна размещаться
            else
            {
                delete part;
            }
        }
        // Если часть за пределами поля - правила размещения корабля были нарушены
        else {
            ship->placementRulesViolated = true;
        }
    }

    // Если у корабля есть части, корабль не нарушает правил либо он фантомный
    if(!ship->parts.empty() && (!ship->placementRulesViolated || isPhantomShip)){
        // Добавить в список кораблей
        ships_.push_back(ship);
        return true;
    }

    // Если корабль не должен быть отрисован на поле
    qDeleteAll(ship->parts);
    delete ship;
    return false;
}

/// H E L P E R S

/**
 * Может ли часть корабля быть зазмешена на поле
 * @param existingParts Массив существующий частей кораблей
 * @return Да или нет
 */
bool ShipPart::canBePlaced(const QVector<ShipPart*>& existingParts)
{
    // Пройтись по всем добавленным на поле частям кораблей
    for(ShipPart* part : existingParts)
    {
        // Расстояние между проверяемой и текущей частью
        QPoint delta = {
                qAbs(part->position.x() - this->position.x()),
                qAbs(part->position.y() - this->position.y())
        };

        // Если часть корабля слишком близко, и ее корабль это не корабль этой части
        if((delta.x() < 2 && delta.y() < 2) && part->ship != this->ship)
        {
            return false;
        }
    }

    return true;
}

/**
 * Нарисовать часть
 * @param painter Объект QtPainter
 * @param existingParts Части кораблей
 * @param cellSize Размер ячейки
 */
void ShipPart::draw(QPainter *painter, const QVector<ShipPart*> &existingParts, qreal cellSize)
{
    // Координаты вершин
    QPoint v0 = {static_cast<int>((this->position.x() + 1) * cellSize), static_cast<int>((this->position.y() + 1) * cellSize)};
    QPoint v1 = {static_cast<int>((this->position.x() + 2) * cellSize), static_cast<int>((this->position.y() + 1) * cellSize)};
    QPoint v2 = {static_cast<int>((this->position.x() + 2) * cellSize), static_cast<int>((this->position.y() + 2) * cellSize)};
    QPoint v3 = {static_cast<int>((this->position.x() + 1) * cellSize), static_cast<int>((this->position.y() + 2) * cellSize)};

    // Части среди которых ведестя поиск соседних
    // Если часть связана с кораблем, то достаточно перебрать его части, иначе нужны все части без корабля
    auto possibleNeighbors = this->ship != nullptr ? this->ship->parts.toStdVector() : GameField::findAllOf(existingParts.toStdVector(),[&](ShipPart* part){
        return part->ship == nullptr;
    });

    // Соседние части
    ShipPart* partUp = GameField::findAt(this->position - QPoint(0,1), possibleNeighbors);
    ShipPart* partRight = GameField::findAt(this->position + QPoint(1,0),possibleNeighbors);
    ShipPart* partBottom = GameField::findAt(this->position + QPoint(0,1),possibleNeighbors);
    ShipPart* partLeft = GameField::findAt(this->position - QPoint(1,0),possibleNeighbors);

    // Верхнее ребро
    if(partUp == nullptr){
        painter->drawLine(v0,v1);
    }

    // Правое ребро
    if(partRight == nullptr){
        painter->drawLine(v1,v2);
    }

    // Нижнее ребро
    if(partBottom == nullptr){
        painter->drawLine(v2,v3);
    }

    // Левое ребро
    if(partLeft == nullptr){
        painter->drawLine(v3,v0);
    }

    // Если часть корабля уничтожена - нарисовать крест
    if(this->isDestroyed){
        painter->drawLine(v0,v2);
        painter->drawLine(v1,v3);
    }
}

/**
 * Получить часть корабля с заданным положением среди частей
 * @param position Положение
 * @param parts Части среди которых искать
 * @return Указатель на часть корабля
 */
ShipPart *GameField::findAt(const QPoint &position, const std::vector<ShipPart*>& parts)
{
    auto elementIt = std::find_if(parts.begin(),parts.end(),[&](ShipPart* entry){
        return entry->position == position;
    });

    if(elementIt == parts.end()){
        return nullptr;
    }

    return *elementIt;
}

/**
 * Полдучить все части удовлетворябщие определнному условию
 * @param parts Массив частей
 * @param condition Услвоие (функция обратного вызкова)
 * @return Массив удоавлетворяющих условию частей
 */
std::vector<ShipPart *> GameField::findAllOf(const std::vector<ShipPart *> &parts, const std::function<bool(ShipPart*)>& condition)
{
    std::vector<ShipPart*> results;
    std::copy_if(parts.begin(), parts.end(), std::back_inserter(results), condition);
    return results;
}



/// E V E N T S

/**
 * Обработчик события нажатия кнопки мыши
 * @param event Событие
 */
void GameField::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

/**
 * Обработчик события движения курсора мыши
 * @param event Событие
 */
void GameField::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
}

/**
 * Обработчик события отпускания кнопки мыши
 * @param event Событие
 */
void GameField::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
}
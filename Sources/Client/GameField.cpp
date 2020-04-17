#include "GameField.h"
#include <QPainter>

/**
 * Конструктор
 * @param cellSize Размер клетки
 */
GameField::GameField(qreal cellSize):cellSize_(cellSize)
{
    // Очистка поля
    cleanField();
}

/**
 * Деструктор
 */
GameField::~GameField()
{
    // Удалчение частей кораблей
    delete[] shipParts_[10][10];
    // Удаление кораблей
    qDeleteAll(ships_);
}

/**
 * Очищает поле (инициализируя все ячейки nullptr значениями)
 */
void GameField::cleanField() {
    memset(shipParts_, 0, sizeof(shipParts_));
}

/**
 * Может ли часть корабля быть размещена на поле в заданном месте
 * @param owner Владеющий частью корабль
 * @param position Положение
 * @return Да или нет
 */
bool GameField::partCanBePlacedAt(Ship *owner, const QPoint& position) {

    // Проход по ячейкам вокруг указанной ячейки (9 ячеек включая саму проверяемую)
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){

            // Текущая ячейка проверки (1 из 9)
            QPoint cellToCheck(position.x() + x - 1, position.y() + y - 1);

            // Если она в допустимых пределах
            if(cellToCheck.x() >= 0 && cellToCheck.x() <= 9 && cellToCheck.y() >= 0 && cellToCheck.y() <= 9){

                // Указатель на часть корабля которая может находиться в этой ячейке
                auto shipPartAtCell = shipParts_[cellToCheck.y()][cellToCheck.x()];

                // Если в ячейке рядом с проверяемой (или на ее месте) есть часть другого корабля, и если корабль находящийся рядом
                // размещается (не фантомный) - размещать нельзя
                if((shipPartAtCell != nullptr && shipPartAtCell->ship != owner) &&
                   (shipPartAtCell->ship != nullptr && shipPartAtCell->ship->canBePlaced))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

/**
 * Добавление корабля
 * @param position Положение начальной ячейки (части) корабля
 * @param orientation Ориентация (относительно начальной ячейки)
 * @param shipLength Длина
 * @param isPhantomShip Фантомный корабль (игнорирует правила размещения, но факт нарушения правил фиксируется)
 * @return Удалось ли добавить корабль на поле
 */
bool GameField::addShip(const QPoint& position, Ship::Orientation orientation, int shipLength, bool isPhantomShip) {

    // Область игрового поля
    QRect gameFieldRect(0,0,10,10);

    // Создать корабль
    auto ship = new Ship;
    ship->orientation = orientation;
    ship->canBePlaced = true;
    ship->isPhantom = isPhantomShip;

    // Построить части корябля и проверить каждую часть возможность ее добавления на поле
    for(int i = 0; i < shipLength; i++)
    {
        // Положение новой части на поле
        QPoint partPosition = (ship->orientation == Ship::HORIZONTAL) ? position + QPoint(i,0) : position + QPoint(0,i);

        // Если часть не за пределами поля
        if(gameFieldRect.contains(partPosition)){
            // Можно ли разместить часть в этом месте
            bool partCanBePlaced = partCanBePlacedAt(ship,partPosition);

            // Если какая-то часть нарушает правила - пометить что корабль не может быть размещен
            if(!partCanBePlaced) ship->canBePlaced = false;

            // Если правила размещения не были нарушены, либо если это фантомный корабль (правила могут нарушаться)
            if(isPhantomShip || partCanBePlaced)
            {
                // Создать часть
                auto part = new ShipPart;
                part->ship = ship;
                part->isBeginning = i == 0;
                part->isDestroyed = false;
                part->position = partPosition;

                // Добавить в корабль
                ship->parts.push_back(part);
            }
        }
        else{
            ship->canBePlaced = false;
        }
    }

    // Если у корабля есть части, корабль не нарушает правил либо он фантомный
    if(!ship->parts.empty() && (ship->canBePlaced || isPhantomShip)){
        // Добавить в список
        ships_.push_back(ship);
        // Добавить части на поле
        for(ShipPart* part : ship->parts){
            shipParts_[part->position.y()][part->position.x()] = part;
        }

        return true;
    }

    // Если корабль не должен быть отрисован на поле
    qDeleteAll(ship->parts);
    delete ship;

    return false;
}

/**
 * Добавление части корабля
 * @param position Положение
 * @param isDestroyed Униточжена
 * @return Удалось ли добавить на поле
 */
bool GameField::addShipPart(const QPoint &position, bool isDestroyed) {

    // Область игрового поля
    QRect gameFieldRect(0,0,10,10);

    // Если за пределами
    if(!gameFieldRect.contains(position)){
        return false;
    }

    // Если не может быть добавлена в связи с правилами размещения
    if(!partCanBePlacedAt(nullptr,position)){
        return false;
    }

    // Создать часть
    auto part = new ShipPart;
    part->position = position;
    part->isDestroyed = isDestroyed;
    part->isBeginning = false;
    part->ship = nullptr;

    shipParts_[part->position.y()][part->position.x()] = part;

    return true;
}


/**
 * Описывающий прямоугольник
 * @return Прямоугольник
 */
QRectF GameField::boundingRect() const {
    return {QPointF(0.0f,0.0f),QPointF(cellSize_ * 11.0f, cellSize_ * 11.0f)};
}

/**
 * Отрисовка объекта
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
    wchar_t symbols[10] = {u'Р',u'Е',u'С',u'П',u'У',u'Б',u'Л',u'И',u'К',u'А'};

    // Нарисовать поле
    for(int i = 0; i < 11; i++)
    {
        for(int j = 0; j < 11; j++)
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
    for(auto& row : shipParts_) {
        for (auto part : row) {

            // Если часть существует
            if(part != nullptr) {

                // Если часть не принадлежит кораблю, любо корабль есть и он НЕ фантомный
                if(part->ship == nullptr || (part->ship != nullptr && !(part->ship->isPhantom)))
                {
                    // Заливка не нужна
                    painter->setBrush(Qt::NoBrush);

                    // Перо (синие линии)
                    painter->setPen(QPen(
                            QColor(0,0,255),
                            4.0f,
                            Qt::PenStyle::SolidLine,
                            Qt::PenCapStyle::SquareCap,
                            Qt::PenJoinStyle::MiterJoin));

                    // Нарисовать часть корабля
                    paintShipPart(painter,part);
                }
                // Если это фантомный корабль (для обозначения)
                else
                {
                    // Отключить карандаш
                    painter->setPen(Qt::NoPen);
                    // Заливка
                    painter->setBrush(part->ship->canBePlaced ? QBrush({0,255,0,100}) : QBrush({255,0,0,100}));
                    // Рисование части
                    painter->drawRect(
                            static_cast<int>((part->position.x() + 1) * cellSize_),
                            static_cast<int>((part->position.y() + 1) * cellSize_),
                            cellSize_,
                            cellSize_);
                }
            }
        }
    }
}

/**
 * Нарисовать часть корабля
 * @param painter Указатель на painter
 * @param point Точка на игровом поле
 */
void GameField::paintShipPart(QPainter *painter, ShipPart* part)
{
    // Координаты вершин
    QPoint v0 = {static_cast<int>((part->position.x() + 1) * cellSize_), static_cast<int>((part->position.y() + 1) * cellSize_)};
    QPoint v1 = {static_cast<int>((part->position.x() + 2) * cellSize_), static_cast<int>((part->position.y() + 1) * cellSize_)};
    QPoint v2 = {static_cast<int>((part->position.x() + 2) * cellSize_), static_cast<int>((part->position.y() + 2) * cellSize_)};
    QPoint v3 = {static_cast<int>((part->position.x() + 1) * cellSize_), static_cast<int>((part->position.y() + 2) * cellSize_)};

    // Соседние ячейки
    ShipPart* partUp = atPoint(part->position - QPoint(0,1));
    ShipPart* partRight = atPoint(part->position + QPoint(1,0));
    ShipPart* partBottom = atPoint(part->position + QPoint(0,1));
    ShipPart* partLeft = atPoint(part->position - QPoint(1,0));

    // Верхнее ребро
    if(partUp == nullptr || partUp->ship != part->ship){
        painter->drawLine(v0,v1);
    }

    // Правое ребро
    if(partRight == nullptr || partRight->ship != part->ship){
        painter->drawLine(v1,v2);
    }

    // Нижнее ребро
    if(partBottom == nullptr || partBottom->ship != part->ship){
        painter->drawLine(v2,v3);
    }

    // Левое ребро
    if(partLeft == nullptr || partLeft->ship != part->ship){
        painter->drawLine(v3,v0);
    }

    // Если часть корабля уничтожена - нарисовать крест
    if(part->isDestroyed){
        painter->drawLine(v0,v2);
        painter->drawLine(v1,v3);
    }
}


/**
 * Получить указатель на часть корабля которая находится в заданной точке, либо nullptr
 * @param point Ячейка поля
 * @return Указатель или nullptr (если части нет, либо точка за пределами поля)
 */
ShipPart *GameField::atPoint(const QPoint &point) {
    // Область игрового поля
    QRect gameFieldRect(0,0,10,10);

    // Если за пределами поля
    if(!gameFieldRect.contains(point)){
        return nullptr;
    }

    // Вернуть указатель на часть (или nullptr)
    return shipParts_[point.y()][point.x()];
}

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

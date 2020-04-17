#pragma once

#include <QGraphicsItem>

/// Часть корабля (объявление)
struct ShipPart;

/**
 * Корабль
 */
struct Ship
{
    /// Ориентация корабля
    enum Orientation {
        HORIZONTAL,
        VERTICAL
    } orientation;

    /// Части корабля
    QVector<ShipPart*> parts;

    /// Может ли корабль быть размещен
    bool canBePlaced = false;

    /// Фантомный корабль (для обозначения)
    bool isPhantom = false;
};

/**
 * Часть корабля
 */
struct ShipPart
{
    /// Корабль
    Ship* ship = nullptr;
    /// Часть уничтожена
    bool isDestroyed = false;
    /// Начальная часть (относительно которой может осуществляться смена ориентации)
    bool isBeginning = false;
    /// Положение на поле
    QPoint position = {};
};

class GameField : public QGraphicsItem {
public:
    /**
     * Конструктор
     * @param cellSize Размер клетки
     */
    explicit GameField(qreal cellSize);

    /**
     * Деструктор
     */
    ~GameField() override;

    /**
     * Описывающий прямоугольник
     * @return Прямоугольник
     */
    QRectF boundingRect() const override;

    /**
     * Отрисовка объекта
     * @param painter Объект painter
     * @param option Параметры рисования
     * @param widget Указатель на виджет, на котором просиходит отрисовка
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    /**
     * Добавление корабля
     * @param position Положение начальной ячейки (части) корабля
     * @param orientation Ориентация (относительно начальной ячейки)
     * @param shipLength Длина
     * @param isPhantomShip Фантомный корабль (игнорирует правила размещения, но факт нарушения правил фиксируется)
     * @return Удалось ли добавить корабль на поле
     */
    bool addShip(const QPoint& position, Ship::Orientation orientation, int shipLength = 1, bool isPhantomShip = false);

    /**
     * Добавление части корабля
     * @param position Положение
     * @param isDestroyed Униточжена
     * @return Удалось ли добавить на поле
     */
    bool addShipPart(const QPoint& position, bool isDestroyed = false);

protected:
    /**
     * Обработчик события нажатия кнопки мыши
     * @param event Событие
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * Обработчик события движения курсора мыши
     * @param event Событие
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * Обработчик события отпускания кнопки мыши
     * @param event Событие
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    /// Размер клетки поля
    qreal cellSize_;
    /// Массив кораблей
    QVector<Ship*> ships_;
    /// Массив частей корабля отображаемых на поле
    ShipPart* shipParts_[10][10] = {{nullptr}};

    /**
     * Очищает поле (инициализируя все ячейки nullptr значениями)
     */
    void cleanField();

    /**
     * Может ли часть корабля быть размещена на поле в заданном месте
     * @param owner Владеющий частью корабль
     * @param position Положение
     * @return Да или нет
     */
    bool partCanBePlacedAt(Ship *owner, const QPoint& position);

    /**
     * Нарисовать часть корабля
     * @param painter Указатель на painter
     * @param part Часть корабля
     */
    void paintShipPart(QPainter* painter, ShipPart* part);

    /**
     * Получить указатель на часть корабля которая находится в заданной точке, либо nullptr
     * @param point Ячейка поля
     * @return Указатель или nullptr (если части нет, либо точка за пределами поля)
     */
    ShipPart* atPoint(const QPoint& point);
};

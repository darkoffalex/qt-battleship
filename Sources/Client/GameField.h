#pragma once

#include <QGraphicsItem>
#include <functional>

/// Часть корабля (объявление)
struct ShipPart;

/// Корабль
struct Ship
{
    // Ориентация корабля
    enum Orientation {
        HORIZONTAL,
        VERTICAL
    } orientation = Orientation::HORIZONTAL;

    // Части корабля
    QVector<ShipPart*> parts;

    // Было нарушение правил размещения корабля

    bool placementRulesViolated = false;

    // Фантомный корабль (для обозначения)
    bool isPhantom = false;
};

/// Часть корабля
struct ShipPart
{
    // Корабль
    Ship* ship = nullptr;

    // Часть уничтожена
    bool isDestroyed = false;

    // Начальная часть (относительно которой может осуществляться смена ориентации)
    bool isBeginning = false;

    // Положение на поле
    QPoint position = {};

    /**
     * Может ли часть корабля быть зазмешена на поле
     * @param existingParts Массив существующий частей кораблей
     * @return Да или нет
     */
    bool canBePlaced(const QVector<ShipPart*>& existingParts);

    /**
     * Нарисовать часть
     * @param painter Объект QtPainter
     * @param existingParts Части кораблей
     * @param cellSize Размер ячейки
     */
    void draw(QPainter* painter, const QVector<ShipPart*>& existingParts, qreal cellSize);
};

/// Метка
struct CellMark
{
    // Тип отметки
    enum CheckType {
        MISS,
        CHECKED
    } type = CheckType::CHECKED;

    // Положение на поле
    QPoint position = {};
};

/// Игровое поле
class GameField final : public QGraphicsItem {

public:
    /**
     * Конструктор
     * @param cellSize Размер ячейки
     * @param fieldSize Размер поля (ячеек по ширине и высоте)
     */
    explicit GameField(qreal cellSize, const QPoint& fieldSize);

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
     * Отрисовка игрового поля со всеми кораблями и прочими объектами
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
     * Получить часть корабля с заданным положением среди частей
     * @param position Положение
     * @param parts Части среди которых искать
     * @return Указатель на часть корабля
     */
    static ShipPart* findAt(const QPoint& position, const std::vector<ShipPart*>& parts);

    /**
     * Полдучить все части удовлетворябщие определнному условию
     * @param parts Массив частей
     * @param condition Услвоие (функция обратного вызкова)
     * @return Массив удоавлетворяющих условию частей
     */
    static std::vector<ShipPart*> findAllOf(const std::vector<ShipPart*>& parts, const std::function<bool(ShipPart*)>& condition);

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

    /// Размер поля
    QPoint fieldSize_;

    /// Массив кораблей
    QVector<Ship*> ships_;

    /// Массив частей кораблей
    QVector<ShipPart*> shipParts_;

    /// Массив отметок клеток
    QVector<CellMark*> cellMarks_;
};
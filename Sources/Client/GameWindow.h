#pragma once

#include <QtWidgets>
#include "GameField.h"

class GameWindow final : public QGraphicsView {
Q_OBJECT
public:
    /**
     * Инициализация игрового окна
     */
    explicit GameWindow(QGraphicsScene* scene);

    /**
     * Деинициализация игрового окна
     */
    ~GameWindow() override;

    /**
     * Выстрел по вражескому полю
     * @param pos Положение клетки по которой осуществляетс выстрел
     * @param gameField Указатель на поле
     */
    static void shotAtEnemy(const QPoint &pos, GameField* gameField);

protected:
    /**
     * Переопределение события изменения размера
     */
    void resizeEvent(QResizeEvent *) override;

private slots:
    /**
     * Обработчик события нажатия на кнопку готовности к игре
     */
    void onReadyButtonClicked();

private:
    /// Игровое поле текущего игрока
    GameField* myField_ = nullptr;
    /// Игровое поле противника
    GameField* enemyField_ = nullptr;
    /// Label'ы для обозначения полей
    QVector<QLabel*> labels_;
    /// Кнопка готовности к игде
    QPushButton* btnReady_ = nullptr;
};
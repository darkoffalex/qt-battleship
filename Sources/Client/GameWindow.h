#pragma once

#include <QtWidgets>
#include "GameField.h"

class GameWindow final : public QGraphicsView{
public:
    /**
     * Инициализация игрового окна
     */
    explicit GameWindow(QGraphicsScene* scene);

    /**
     * Деинициализация игрового окна
     */
    ~GameWindow() override;

protected:
    /**
     * Переопределение события изменения размера
     */
    void resizeEvent(QResizeEvent *) override;

private:
    /// Игровое поле
    GameField* gameField_;
};
#include "GameWindow.h"

/**
 * Инициализация игрового окна
 */
GameWindow::GameWindow(QGraphicsScene* scene) : gameField_(new GameField(30.0f,{10,10})), QGraphicsView(scene)
{
    // Основные настройки рендеринга и окна
    this->setRenderHint(QPainter::Antialiasing);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setBackgroundBrush(QColor(230, 200, 167));
    this->setWindowTitle("Battleship");

    // Добавить игровое поле к отрисовке
    this->scene()->addItem(gameField_);
}

/**
 * Деинициализация игрового окна
 */
GameWindow::~GameWindow()
{
    this->scene()->removeItem(gameField_);
    delete gameField_;
}

/**
 * Переопределение события изменения размера
 * @details Достаточно просто оставить обработчик пустым
 */
void GameWindow::resizeEvent(QResizeEvent *) {}

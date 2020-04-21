#include "GameWindow.h"
#include <iostream>

/**
 * Инициализация игрового окна
 */
GameWindow::GameWindow(QGraphicsScene* scene) : QGraphicsView(scene)
{
    // Основные настройки рендеринга и окна
    this->setRenderHint(QPainter::Antialiasing);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setBackgroundBrush(QColor(230, 200, 167));
    this->setWindowTitle("Battleship");

    // Создать игровое поле для игрока
    myField_ = new GameField(30.0,{10,10},GameField::FieldState::PREPARING);
    myField_->setPos(0,30);
    myField_->addStartupShips();

    // Создать игровое поле для противника
    enemyField_ = new GameField(30.0,{10,10},GameField::FieldState::ENEMY_PREPARING);
    enemyField_->setPos(360,30);
    enemyField_->setShotAtEnemyCallback(GameWindow::shotAtEnemy);

    // Создать label'ы
    QPalette labelPal(palette());
    labelPal.setColor(QPalette::Background, QColor(0,0,0,0));
    for(size_t i = 0; i < 2; i++)
    {
        auto label = new QLabel();
        label->setText(i == 0 ? "Ваше поле" : "Поле противника");
        label->setFont(QFont("Arial",18));
        label->setAutoFillBackground(true);
        label->move(i == 0 ? 30 : 390,7);
        label->setPalette(labelPal);
        labels_.push_back(label);
    }

    // Создать кнопку готовности к игре
    btnReady_ = new QPushButton;
    btnReady_->setText("Готов к игре!");
    btnReady_->setFont(QFont("Arial",18));
    btnReady_->move(390,60);

    // Связать кнопку с обработчиком события
    connect(btnReady_,&QPushButton::clicked,this,&GameWindow::onReadyButtonClicked);

    // Добавить игровые поля к отрисовке
    this->scene()->addItem(myField_);
    this->scene()->addItem(enemyField_);

    // Добавить label'ы к отрисовке
    for(auto label : labels_){
        this->scene()->addWidget(label);
    }

    // Добавить кнопку к отрисовке
    this->scene()->addWidget(btnReady_);
}

/**
 * Деинициализация игрового окна
 */
GameWindow::~GameWindow()
{
    // Удаление объейтов со сцены
    this->scene()->removeItem(myField_);
    this->scene()->removeItem(enemyField_);

    // Уничтожение объектов
    qDeleteAll(labels_);
    delete btnReady_;
    delete myField_;
    delete enemyField_;
}

/**
 * Переопределение события изменения размера
 * @details Достаточно просто оставить обработчик пустым
 */
void GameWindow::resizeEvent(QResizeEvent *) {}

/// S L O T S

/**
 * Обработчик события нажатия на кнопку готовности к игре
 */
void GameWindow::onReadyButtonClicked()
{
    // Получить отправителя события (сигнала)
    auto button = qobject_cast<QPushButton*>(sender());

    // Если все корабли размещены
    if(myField_ != nullptr && myField_->allShipsPlaced())
    {
        //TODO: Вывод дополнительного лиалогового окна, в котором игрок либо подключается к существующей сесси, либо создает новую
        //TODO: Обработка ответа от сервера, сменя состояния полей полсе успешного ответа сервера
        myField_->setState(GameField::FieldState::READY);
        enemyField_->setState(GameField::FieldState::ENEMY_READY);
        button->setVisible(false);
    }
    // Если не все корабли размещены на поле
    else
    {
        // Сообщение
        QMessageBox msgBox;
        msgBox.setWindowTitle("Внимание!");
        msgBox.setText("Игру невозможно начать, пока все корабли не будут размещены на поле.");
        msgBox.setIcon(QMessageBox::Icon::Warning);
        msgBox.exec();
    }
}

/**
 * Выстрел по вражескому полю
 * @param pos Положение клетки по которой осуществляетс выстрел
 * @param gameField Указатель на поле
 */
void GameWindow::shotAtEnemy(const QPoint &pos, GameField *gameField)
{
    //TODO: отправка данных на сервер и получение ответа
}

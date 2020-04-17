#include <QtWidgets>

#include "GameWindow.h"

/**
 * Точка входа
 * @param argc Кол-во аргументов
 * @param argv Аргументы
 * @return Код выполнения (выхода)
 */
int main(int argc, char* argv[])
{
    // Инициализация QT
    QApplication app(argc, argv);

    // Игровое окно
    QGraphicsScene scene(0, 0, 800, 600);
    GameWindow gameWindow(&scene);
    gameWindow.show();

    // Исполнение приложение и обработка всех соыбтий
    return QApplication::exec();
}
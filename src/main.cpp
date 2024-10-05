/* STANDARDOWE BIBLIOTEKI */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
/* GRAFIKA/DŹWIĘK */
#include <SFML/Graphics.hpp>  // Do grafiki 2D
#include <SFML/Audio.hpp>     // Do dźwięku
#include <SFML/System.hpp>    // Do zarządzania czasem i wątkami
#include <SFML/Window.hpp>    // Do obsługi okien i zdarzeń
/* AI */
#include <tensorflow/core/public/session.h>      // Główna sesja TensorFlow
#include <tensorflow/core/protobuf/meta_graph.pb.h> // Do odczytu modelu
/* WIELOWĄTKOWOŚĆ */
#include <thread>    // Do tworzenia i zarządzania wątkami
#include <mutex>     // Do synchronizacji wątków
#include <condition_variable>  // Do bardziej złożonej synchronizacji
/* JSON */
#include <nlohmann/json.hpp>   // Do obsługi plików JSON
/* DEBUGOWANIE */
using std::cout;
using std::endl;

auto main() -> int {
    cout << "Hello, World :o" << endl;
    return 0;
}
// Home_3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <fstream>
#include <climits>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <time.h>
#include <thread>
#include <algorithm>
#include <string>
#include <vector>
// Groups of Pirates
std::vector<int> groups;
// Our Threads
std::vector<std::thread> threads;
// Tasks for threads
std::vector<int> tasks;
// Free Threads
std::queue<int> FreeThreads;
// Our island
std::vector<std::string> island;
// island Height
int Height;
// island Width
int Width;
// Number of item, having a treasure
int treasureNumber;
// Have we already find the treasure
bool found = false;
// Group that find the treasure
int GroupOfFounders;
// mutex print
std::mutex print;
// mutex tasks
std::mutex mut_tasks;
// mutex mut
std::mutex mut;
// the end of the thread
std::condition_variable thread_cv;
// a new task for Pirates Group
std::condition_variable task_cv;
// Treasure has beed found
std::condition_variable found_cv;
// My own stoi method (returns bool)
bool stoi(const std::string& str, int* p_value, std::size_t* pos = 0, int base = 10) {
    // if its int, return true
    try {
        *p_value = std::stoi(str, pos, base);
        return true;
    }
    // else catch the exception return false
    catch (const std::exception& e) {
        return false;
    }
}
// Read number and cheack it for correct input
int GetInt(int min = INT_MIN, int max = 3000) {
    int result;
    bool correctInput = false;
    std::string tmp;
    std::cin >> tmp;
    // check by method 
    correctInput = stoi(tmp, &result);
// Do while the input is uncorrect
    while (!correctInput || std::to_string(result) != tmp || result < min || result > max) {
        std::cout << "Incorrect input. Try again \n";
        std::cin >> tmp;
        correctInput = stoi(tmp, &result);
    }
    return result;
}
// output of island's items, with information about them
void MyIsland() {
    std::cout << "Island: \n";
    // cross all island's items
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            std::cout << island[i * Width + j] << " ";
        }
        std::cout << std::endl;
    }
}
// put the treasure in a random place on the island and get the initial data
void Start_One() {
    // input 0 values in our vectors
    groups.push_back(0);
    tasks.push_back(0);
    std::cout << "Input the Height of the island (starts from 1): \n";
        Height = GetInt(1);
    std::cout << "Input the Width of the island (starts from 1): \n";
        Width = GetInt(1);
    std::cout << std::endl;
    // Check the Max int value
    if (Height * Width > INT_MAX) {
        std::cout << "Can't goes next step. The programm will ends\n";
        exit(0);
    }    
    // Put 0 in our island items
    for (int i = 0; i < Height * Width; i++) {
        island.push_back("0");
    }
    // Generate the island in conlose
    MyIsland();
    std::cout << std::endl;
    std::cout << "Enter the number of Pirates (starts from 1): \n";
    int pirates = GetInt(1);
    int Pirates_free = pirates;
    std::cout << std::endl;
    // While there are still free Pirate, group them up
    for (int i = 0; ; ++i) {
        if (Pirates_free == 0) {
            break;
        }
        std::cout << "Number of free Pirates = " << Pirates_free << " \n";
        std::cout << "Enter the number of Pirates, from what the group " << i + 1 << " will be organized: (from 1 to " << Pirates_free << ")\n";
        int currentGroup = GetInt(1, Pirates_free);
        // enter droups in our vector
        groups.push_back(currentGroup);
        Pirates_free -= currentGroup;
    }
    std::cout << "\nGroups of Pirates are confirmed: \n";
    for (int i = 1; i < groups.size(); ++i) {
        std::cout << groups[i] << " ";
    }
    std::cout << "Starting the program \n\n";
    // Generate the random treasure item place
    srand(time(0));
    treasureNumber = std::rand() % (Width * Height);
}
void Start_Two() {
    // input 0 values in our vectors
    groups.push_back(0);
    tasks.push_back(0);
    std::cout << "You are already choose the Height of the island : " << Height << "\n";
    std::cout << "You are already choose the Width of the island : " << Width << "\n";
    std::cout << std::endl;
    // Check the Max int value
    if (Height * Width > INT_MAX) {
        std::cout << "Can't goes next step. The programm will ends\n";
        exit(0);
    }
    // Put 0 in our island items
    for (int i = 0; i < Height * Width; i++) {
        island.push_back("0");
    }
    // Generate the island in conlose
    MyIsland();
    std::cout << std::endl;
    std::cout << "Enter the number of Pirates (starts from 1): \n";
    int pirates = GetInt(1);
    int Pirates_free = pirates;
    std::cout << std::endl;
    // While there are still free Pirate, group them up
    for (int i = 0; ; ++i) {
        if (Pirates_free == 0) {
            break;
        }
        std::cout << "Number of free Pirates = " << Pirates_free << " \n";
        std::cout << "Enter the number of Pirates, from what the group " << i + 1 << " will be organized : (from 1 to " << Pirates_free << ")\n";
        int currentGroup = GetInt(1, Pirates_free);
        // enter droups in our vector
        groups.push_back(currentGroup);
        Pirates_free -= currentGroup;
    }
    std::cout << "\nGroups of Pirates are confirmed: \n";
    for (int i = 1; i < groups.size(); ++i) {
        std::cout << groups[i] << " ";
    }
    std::cout << "Starting the program \n\n";
    // Generate the random treasure item place
    srand(time(0));
    treasureNumber = std::rand() % (Width * Height);
}
// function, controlls the Groups of Pirates behaviour
void PiratesQuests(int tNumber, int members) {
    while (true) {
        {
            // while there are no quests
            std::unique_lock<std::mutex> ul(mut);
            task_cv.wait(ul, [tNumber] {return tasks[tNumber] != -1;  });
        }
        int task = tasks[tNumber];
        // Stop the searching!
        if (task == -2) {
            return;
        }
        {
            // Enter in console information about group starting searching
            std::unique_lock<std::mutex> ulp(print);
            std::cout << "Group " << tNumber << " Begin to search the item " << task << "\n";
        }
        // Sleep the thread for some secconds (depends on the number of Pirates in each Group
        std::this_thread::sleep_for(std::chrono::seconds(7 / members));
        std::unique_lock<std::mutex> ul(mut_tasks);
        // If the Group have found the item
        if (task == treasureNumber) {
            island[task] = "X";
            {
                std::unique_lock<std::mutex> ulp(print);
                std::cout << "\n Group " << tNumber << " stop to search the item " << task << "\n";
                std::cout << "The TREASURE was founded!!!\n";
                // Entering island in console
                MyIsland();
                std::cout << std::endl;
            }
            GroupOfFounders = tNumber;
            FreeThreads.push(tNumber);
            thread_cv.notify_one();
            found_cv.notify_one();
            found = true;
            return;
        }
        // If the Group have not found the item
        island[task] = "_";
        {
            std::unique_lock<std::mutex> ulp(print);
            std::cout << "\n Group " << tNumber << " stop to search the item " << task << "\n";
            // Entering island in console
            MyIsland();
            std::cout << std::endl;
        }
        if (tasks[tNumber] != -2) {
            tasks[tNumber] = -1;
        }
        FreeThreads.push(tNumber);
        thread_cv.notify_one();
    }
}
int main(int argc, char* argv[]) {
    // Check the input
    if (argc == 3) {
        std::cout << "Start of the Programm (with input arguments)\n" << "\n";
        Height = atoi(argv[1]);
        Width = atoi(argv[2]);
        if (Height <= 0 || Width <= 0) {
            std::cout << "Incorrect input, Program will end \ntry again!";
            return 1;
        }
        Start_Two();
    }
    else {
        // Start of the Programm (without input arguments)
        std::cout << "Start of the Programm (without input arguments)\n" << "\n";
        Start_One();
    }
    for (int i = 1; i < std::min((int)groups.size(), Height * Width + 1); i++) {
        // Get the threads without quests
        std::thread t(PiratesQuests, i, groups[i]);
        threads.push_back(std::move(t));        
        FreeThreads.push(i);
        tasks.push_back(-1);
    }
    for (int i = 0; i < Height * Width; i++) {
        {
            // wait for empty thread
            std::unique_lock<std::mutex> ul(mut_tasks);
            thread_cv.wait(ul, [] {return !FreeThreads.empty(); });
        }
        {
            // stop if the treasure is already found
            if (found) break;
            // Print the information about group (thread) action
            std::unique_lock<std::mutex> ul(print);
            std::cout << "\nGroup " << FreeThreads.front() << " get new quest in the item " << i << "\n";
        }
        {
            // the group(thread) gets the quest and became busy
            std::unique_lock<std::mutex> ul(mut_tasks);
            tasks[FreeThreads.front()] = i;
            FreeThreads.pop();
        }
        task_cv.notify_all();
    }
    {
        std::unique_lock<std::mutex> ul(mut_tasks);
        found_cv.wait(ul, [] {return found; });
    }
    {
        // ends all quests
        std::unique_lock<std::mutex> ul(mut_tasks);
        for (int i = 0; i < tasks.size(); i++) {
            tasks[i] = -2;
        }
    }
    task_cv.notify_all();
    for (std::thread& th : threads) {
        th.join();
    }
    std::cout << "Well done \n";
    std::cout << "The questions are over! \n";
    std::cout << "TREASURE has been found by the group " << GroupOfFounders << "\n";
    std::cout << "Program ENDS here \n";
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

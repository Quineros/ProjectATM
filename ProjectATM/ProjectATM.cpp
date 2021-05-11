//Программа реализует работу банкомата, позволяя пользователю снимать определенную сумму денег
//различными способами (крупными купюрами, мелкими купюрами, с разменом).
//Предусмотрены наличие дневного лимита снятия наличных, лимита единовременно выдаваемых средств и лимита количества выдаваемых за раз купюр.
//При невозможности выполнения какой-либо операции пользователю не предоставляется возможность выбрать ее в меню.

#include <iostream>
#include <vector>
using namespace std;

int ConfigBills[][2] = { {100,10},{200,10},{500,10},{1000,10},{2000,10},{5000,10} }; // Значения номиналов и количества доступных купюр
int StandardSum[] = { 100, 500, 3000 };  // Стандартные суммы вывода денег

int UserBalance = 1000;

int dailylimit = 10000, oneinstancelimit = 10000, billlimit = 10; // Лимиты выдачи денег
int DLcnt = 0, BLcnt = 0;  //Счетчики достижения лимитов

bool endsession = 0, dailylimitreached = 0;

struct bill {
    int value, amount, index; // Структура пачки купюр, содержащая номинал, количество, и индекс этих купюр в массиве ConfigBills
};

bool EqualBillVectors(vector<bill> a, vector<bill> b) { // Функция проверки равенства двух векторов структур bill
    bool equal = 1;
    for (int i = 0; i < size(a); i++) {
        if (a[i].amount != b[i].amount) {
            equal = 0;
            break;
        }
    }
    return equal;
}

vector<bill> LargeBills(int need) { //Функция нахождения способа выдачи денег крупными купюрами
    //Алгоритм: Проход по массиву доступных купюр, начиная с самого крупного номинала, добавление купюр к выдаче, пока не превышена необходимая сумма выдачи.

    if ((DLcnt + need) > dailylimit) {
        return {};
    }                                  //Проверки на превышение лимитов
    if (need > oneinstancelimit) {
        return{};
    }
    BLcnt = 0;


    int BillsBackup[size(ConfigBills)];
    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1];  //Сохранение текущего состояния купюр в банкомате

    vector<bill> output;
    bill billbuff, emptybill;
    emptybill.value = 0;
    emptybill.amount = 0;
    billbuff = emptybill;
    bool canbegiven = 0;


    for (int i = size(ConfigBills) - 1; i >= 0; i--) {  //Проход по доступным в банкомате купюрам, начиная с самых крупных
        billbuff.value = ConfigBills[i][0];
        billbuff.index = i;
        while ((need >= ConfigBills[i][0]) and (ConfigBills[i][1] > 0)) { //Пока нужная сумма больше либо равна номиналу текущей купюры, добавление этой купюры к результату
            need -= ConfigBills[i][0]; //Уменьшение нужной суммы на номинал текущей купюры
            ConfigBills[i][1]--;
            billbuff.amount++;
            canbegiven = 1;
        }
        output.push_back(billbuff);
        BLcnt += billbuff.amount;

        if (need > 0) canbegiven = 0;

        billbuff.value = 0;
        billbuff.amount = 0;
        billbuff.index = 0;
    }
    if (BLcnt > billlimit) canbegiven = 0; //Проверка на превышение лимита количества единовременно выдаваемых купюр
    if (canbegiven) return output;
    else {
        for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i]; //Возвращение состояния купюр банкомата в исходное при невозможности выдачи денег
        return {};
    }
}


vector<bill> Change(int need) { //Функция размена одной купюры на меньшие (но все еще максимально крупные)
    //Алгоритм: Проход по массиву доступных купюр, начиная с самой крупной купюры, меньшей нужной суммы, добавление купюр к выдаче, пока не превышена нужная сумма.

    int originalneed = need;
    int BillsBackup[size(ConfigBills)];
    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1]; //Сохранение текущего состояния купюр в банкомате

    vector<bill> output;
    bill billbuff, emptybill;
    emptybill.value = 0;
    emptybill.amount = 0;
    billbuff = emptybill;
    bool canbegiven = 0;
    int billsamount = 0;


    for (int i = size(ConfigBills) - 1; i >= 0; i--) {  //Проход по доступным в банкомате купюрам, начиная с самых крупных
        billbuff.value = ConfigBills[i][0];
        billbuff.index = i;
        while ((need >= ConfigBills[i][0]) and (ConfigBills[i][1] > 0) and (ConfigBills[i][0] != originalneed)) { //Пока разница между текущей суммой и требуемой больше номинала текущей купюры, и тот не равен требуемой сумме, добавление этой купюры к результату
            need -= ConfigBills[i][0];
            ConfigBills[i][1]--;
            billbuff.amount++;
            canbegiven = 1;
        }

        output.push_back(billbuff);

        if (need > 0) canbegiven = 0;

        billbuff.value = 0;
        billbuff.amount = 0;
        billbuff.index = 0;
    }

    for (int i = 0; i < size(output); i++) {                       //
        billsamount += output[i].amount;                           //
    }                                                              //
    if ((BLcnt + billsamount) > (billlimit + 1)) canbegiven = 0;   //Проверка на превышение лимита количества единовременно выдаваемых купюр

    if (canbegiven) return output;
    else {
        for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i]; //Возвращение состояния купюр банкомата в исходное при невозможности выдачи денег
        return {};
    }
}

vector<bill> SmallBills(int need) { //Функция нахождения способа выдачи денег мелкими купюрами
    //Алгоритм: Получение способа выдачи нужной суммы крупными купюрами, а затем размен каждой размениваемой из них (начиная с самой крупной) на мелкие.

    BLcnt = 0;
    if (need > oneinstancelimit) {
        return{};
    }                                 //Проверки на превышение лимитов
    if ((DLcnt + need) > dailylimit) {
        return {};
    }

    int BillsBackup[size(ConfigBills)];
    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1]; //Сохранение текущего состояния купюр в банкомате


    vector<bill> output, billchange, previousoutput;
    int previousBLcnt;
    int currbillamount = 0;

    output = LargeBills(need); //Получение способа выдачи нужного количества денег максимально крупными купюрами

    if (!(output.empty())) {
        for (int i = 0; i < output.size() - 1; i++) { //Размен по возможности каждой купюры, начиная с самой крупной

            currbillamount = output[i].amount;

            for (int k = 0; k < currbillamount; k++) { //Проход по всем купюрам текущего номинала

                previousoutput = output;  //Сохранение предыдущих значений результата и счетчика лимита купюр
                previousBLcnt = BLcnt;

                billchange = Change(output[i].value); //Размен текущей купюры

                if (!(billchange.empty())) {

                    output[i].amount--;
                    ConfigBills[output[i].index][1]++;

                    for (int j = 0; j < billchange.size(); j++) {
                        output[j].amount += billchange[j].amount;
                    }
                }

                BLcnt = 0;
                for (int z = 0; z < size(output); z++) {
                    BLcnt += output[z].amount;
                }
                if ((BLcnt > billlimit) and (previousBLcnt <= billlimit)) output = previousoutput; //Возврат к предвдущему значению при превышении лимита единовременной выдачи купюр
            }
        }
    }
    if (output.empty()) for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i]; //Возвращение состояния купюр банкомата в исходное при невозможности выдачи денег
    return output;
}

vector<bill> BillsWChange(int need) { //Функция нахождения способа выдачи денег с разменом купюр
    //Алгоритм: Получение способа выдачи нужной суммы крупными купюрами, а затем размен каждой одной купюры различных номиналов (начиная с самой крупной) на мелкие.

    BLcnt = 0;
    if (need > oneinstancelimit) { //Проверки на превышение лимитов
        return{};
    }
    if ((DLcnt + need) > dailylimit) {
        return {};
    }


    int BillsBackup[size(ConfigBills)];
    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1]; //Сохранение текущего состояния купюр в банкомате

    vector<bill> output, billchange, previousoutput;
    int previousBLcnt;

    output = LargeBills(need); //Получение способа выдачи нужного количества денег максимально крупными купюрами

    if (!(output.empty())) {
        for (int i = 0; i < output.size() - 1; i++) { //Размен одной из купюр каждого номинала на меньшие
            if (output[i].amount > 0) {

                previousoutput = output; //Сохранение предыдущих значений результата и счетчика лимита купюр
                previousBLcnt = BLcnt;

                billchange = Change(output[i].value); //Размен текущей купюры

                if (!(billchange.empty())) {
                    output[i].amount--;
                    ConfigBills[output[i].index][1]++;
                    for (int j = 0; j < billchange.size(); j++) {
                        output[j].amount += billchange[j].amount;
                    }
                }

                BLcnt = 0;
                for (int z = 0; z < size(output); z++) {
                    BLcnt += output[z].amount;
                }
                if ((BLcnt > billlimit) and (previousBLcnt <= billlimit)) output = previousoutput; //Возврат к предвдущему значению при превышении лимита единовременной выдачи купюр
            }
        }
    }
    if (output.empty()) for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i]; //Возвращение состояния купюр банкомата в исходное при невозможности выдачи денег
    return output;
}

void BillsMenu(int money) { //Меню выбора формата выдачи денег

    int BillsBackup[size(ConfigBills)];
    vector<bill> smallbill;
    vector<bill> largebill;
    vector<bill> billwchange;
    bool invalidcommand = 1;
    int menucnt = 1;
    int billsmenuchoice;
    bool smallavailable = 0, changeavailable = 0, largeavailable = 0;
    int endsessionchoice;

    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1];
    smallbill = SmallBills(money);                     //Проверка возможности выдачи нужной суммы мелкими купюрами
    for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i];                                  //Возвращение купюр в банкомате в исходное состояние

    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1];
    largebill = LargeBills(money);                     //Проверка возможности выдачи нужной суммы крупными купюрами
    for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i];                                  //Возвращение купюр в банкомате в исходное состояние

    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1];
    billwchange = BillsWChange(money);                 //Проверка возможности выдачи нужной суммы с разменом
    for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i];                                  //Возвращение купюр в банкомате в исходное состояние

    cout << "Доступные купюры:  ";
    for (int i = 0; i < size(ConfigBills); i++) {
        if (ConfigBills[i][1] > 0) cout << "  " << ConfigBills[i][0] << '(' << ConfigBills[i][1] << ')'; //Вывод доступных купюр
    }
    cout << endl;
    cout << "Баланс: " << UserBalance << " рублей" << endl; //Вывод баланса пользователя
    cout << endl << endl;


    if ((!(smallbill.empty())) and (!(EqualBillVectors(smallbill, largebill)))) {   //Вывод доступных вариантов выдачи нужной суммы
        smallavailable = 1;
        cout << menucnt << " – Выдать сумму мелкими купюрами" << endl;
        menucnt++;
    }
    if ((!(billwchange.empty())) and (!(EqualBillVectors(billwchange, largebill)))) {
        changeavailable = 1;
        cout << menucnt << " – Выдать сумму с разменом купюр" << endl;
        menucnt++;
    }
    if ((!(EqualBillVectors(smallbill, largebill))) and (!(EqualBillVectors(billwchange, largebill)))) {
        largeavailable = 1;
        cout << menucnt << " – Выдать сумму крупными купюрами" << endl;
    }


    if ((EqualBillVectors(smallbill, largebill)) and (EqualBillVectors(billwchange, largebill))) { //Моментальная выдача денег, если вариант выдачи всего один
        UserBalance -= money;
        cout << "Возьмите купюры:    ";
        for (int i = 0; i < size(largebill); i++) {
            if (largebill[i].amount > 0) cout << largebill[i].value << '(' << largebill[i].amount << ')' << "  ";
        }
        cout << endl;
        largebill = LargeBills(money);
        cout << "Завершить сеанс?" << endl; //Возможность завершить сеанс или продолжить пользование банкоматом
        cout << "1 – Да" << endl;
        cout << "2 – Нет" << endl;
        while (true) { //Ожидание ответа пользователя
            try {
                cin >> endsessionchoice;
            }
            catch (invalid_argument) {
                cout << "Неопознанная команда, повторите ввод" << endl;
                continue;
            }
            if (endsessionchoice == 1) { //Завершение работы банкомата
                endsession = 1;
                return;
            }
            else if (endsessionchoice == 2) { //Возврат в главное меню
                DLcnt += money;
                return;
            }
            else cout << "Неопознанная команда, повторите ввод" << endl;
        }
    }
    while (invalidcommand) {
        invalidcommand = 1;
        try {
            cin >> billsmenuchoice; //Ввод формата выдачи денег
        }
        catch (invalid_argument) {
            cout << "Неопознанная команда, повторите ввод" << endl;
            continue;
        }

        if (billsmenuchoice == 1) { //Рассмотрение выбора пользователя с учетом того, что при различных ситуациях операции имеют разные номера пунктов меню
            invalidcommand = 0;

            if (smallavailable) {
                UserBalance -= money;
                cout << "Возьмите купюры:    "; //Выдача денег мелкими купюрами
                for (int i = 0; i < size(smallbill); i++) {
                    if (smallbill[i].amount > 0) cout << smallbill[i].value << '(' << smallbill[i].amount << ')' << "  ";
                }
                cout << endl;
                smallbill = SmallBills(money);
                cout << "Завершить сеанс?" << endl; //Возможность завершить сеанс или продолжить пользование банкоматом
                cout << "1 – Да" << endl;
                cout << "2 – Нет" << endl;
                while (true) { //Ожидание ответа пользователя
                    try {
                        cin >> endsessionchoice;
                    }
                    catch (invalid_argument) {
                        cout << "Неопознанная команда, повторите ввод" << endl;
                        continue;
                    }
                    if (endsessionchoice == 1) { //Завершение работы банкомата
                        endsession = 1;
                        return;
                    }
                    else if (endsessionchoice == 2) { //Возврат в главное меню
                        DLcnt += money;
                        return;
                    }
                    else cout << "Неопознанная команда, повторите ввод" << endl;
                }
            }
            else {
                UserBalance -= money;
                cout << "Возьмите купюры:    ";
                for (int i = 0; i < size(billwchange); i++) { //Выдача денег с разменом
                    if (billwchange[i].amount > 0) cout << billwchange[i].value << '(' << billwchange[i].amount << ')' << "  ";
                }
                cout << endl;
                billwchange = BillsWChange(money);
                cout << "Завершить сеанс?" << endl; //Возможность завершить сеанс или продолжить пользование банкоматом
                cout << "1 – Да" << endl;
                cout << "2 – Нет" << endl;
                while (true) {  //Ожидание ответа пользователя
                    try {
                        cin >> endsessionchoice;
                    }
                    catch (invalid_argument) {
                        cout << "Неопознанная команда, повторите ввод" << endl;
                        continue;
                    }
                    if (endsessionchoice == 1) { //Завершение работы банкомата
                        endsession = 1;
                        return;
                    }
                    else if (endsessionchoice == 2) { //Возврат в главное меню
                        DLcnt += money;
                        return;
                    }
                    else cout << "Неопознанная команда, повторите ввод" << endl;
                }
            }
        }
        else if (billsmenuchoice == 2) { //Рассмотрение выбора пользователя с учетом того, что при различных ситуациях операции имеют разные номера пунктов меню
            invalidcommand = 0;
            if (changeavailable) {
                UserBalance -= money;
                cout << "Возьмите купюры:    ";
                for (int i = 0; i < size(billwchange); i++) { //Выдача денег с разменом
                    if (billwchange[i].amount > 0) cout << billwchange[i].value << '(' << billwchange[i].amount << ')' << "  ";
                }
                cout << endl;
                billwchange = BillsWChange(money);
                cout << "Завершить сеанс?" << endl; //Возможность завершить сеанс или продолжить пользование банкоматом
                cout << "1 – Да" << endl;
                cout << "2 – Нет" << endl;
                while (true) { //Ожидание ответа пользователя
                    try {
                        cin >> endsessionchoice;
                    }
                    catch (invalid_argument) {
                        cout << "Неопознанная команда, повторите ввод" << endl;
                        continue;
                    }
                    if (endsessionchoice == 1) { //Завершение работы банкомата
                        endsession = 1;
                        return;
                    }
                    else if (endsessionchoice == 2) { //Возврат в главное меню
                        DLcnt += money;
                        return;
                    }
                    else cout << "Неопознанная команда, повторите ввод" << endl;
                }
            }
            else {
                UserBalance -= money;
                cout << "Возьмите купюры:    "; //Выдача денег крупными купюрами
                for (int i = 0; i < size(largebill); i++) {
                    if (largebill[i].amount > 0) cout << largebill[i].value << '(' << largebill[i].amount << ')' << "  ";
                }
                cout << endl;
                largebill = LargeBills(money);
                cout << "Завершить сеанс?" << endl; //Возможность завершить сеанс или продолжить пользование банкоматом
                cout << "1 – Да" << endl;
                cout << "2 – Нет" << endl;
                while (true) { //Ожидание ответа пользователя
                    try {
                        cin >> endsessionchoice;
                    }
                    catch (invalid_argument) {
                        cout << "Неопознанная команда, повторите ввод" << endl;
                        continue;
                    }
                    if (endsessionchoice == 1) { //Завершение работы банкомата
                        endsession = 1;
                        return;
                    }
                    else if (endsessionchoice == 2) { //Возврат в главное меню
                        DLcnt += money;
                        return;
                    }
                    else cout << "Неопознанная команда, повторите ввод" << endl;
                }
            }
        }
        else if (billsmenuchoice == 3) { //Рассмотрение выбора пользователя с учетом того, что при различных ситуациях операции имеют разные номера пунктов меню
            invalidcommand = 0;
            UserBalance -= money;
            cout << "Возьмите купюры:    "; //Выдача денег крупными купюрами
            for (int i = 0; i < size(largebill); i++) {
                if (largebill[i].amount > 0) cout << largebill[i].value << '(' << largebill[i].amount << ')' << "  ";
            }
            cout << endl;
            largebill = LargeBills(money);
            cout << "Завершить сеанс?" << endl; //Возможность завершить сеанс или продолжить пользование банкоматом
            cout << "1 – Да" << endl;
            cout << "2 – Нет" << endl;
            while (true) { //Ожидание ответа пользователя
                try {
                    cin >> endsessionchoice;
                }
                catch (invalid_argument) {
                    cout << "Неопознанная команда, повторите ввод" << endl;
                    continue;
                }
                if (endsessionchoice == 1) { //Завершение работы банкомата
                    endsession = 1;
                    return;
                }
                else if (endsessionchoice == 2) { //Возврат в главное меню
                    DLcnt += money;
                    return;
                }
                else cout << "Неопознанная команда, повторите ввод" << endl;
            }
        }
        else {
            cout << "Неопознанная команда, повторите ввод" << endl;
            continue;
        }
    }
}

void SumMenu() { //Меню выбора выдаваемой суммы денег
    int BillsBackup[size(ConfigBills)];
    vector<int> standardsum;
    vector<bill> standardsumtest;
    vector<bill> customsumtest;
    int summenuchoice;
    int finalsum;
    int smallestavailablebill = ConfigBills[size(ConfigBills) - 1][0];
    for (int i = 0; i < size(ConfigBills); i++) { //Нахождение наименьшей из доступных купюр
        if (ConfigBills[i][1] > 0) {
            smallestavailablebill = ConfigBills[i][0];
            break;
        }
    }

    for (int i = 0; i < size(StandardSum); i++) { //Проверка возможности выдачи стандартных сумм
        for (int j = 0; j < size(ConfigBills); j++) BillsBackup[j] = ConfigBills[j][1];
        standardsumtest = LargeBills(StandardSum[i]);
        for (int j = 0; j < size(ConfigBills); j++) ConfigBills[j][1] = BillsBackup[j];
        if (!(standardsumtest.empty())) standardsum.push_back(StandardSum[i]);
    }
    if ((DLcnt + smallestavailablebill) > dailylimit) { //Проверка на достижение дневного лимита выдачи денег
        dailylimitreached = 1;
        return;
    }
    cout << "Доступные купюры:  "; //Вывод доступных в банкомате купюр
    for (int i = 0; i < size(ConfigBills); i++) {
        if (ConfigBills[i][1] > 0) cout << "  " << ConfigBills[i][0] << '(' << ConfigBills[i][1] << ')';
    }
    cout << endl;
    cout << "Баланс: " << UserBalance << " рублей" << endl; //Вывод баланса клиента
    cout << endl << endl;
    int menucnt = 1;

    for (int i = 0; i < size(standardsum); i++) {
        cout << (i + 1) << " – Снять " << standardsum[i] << " рублей" << endl;  //Вывод доступных для снятия стандартных сумм
        menucnt++;
    }

    cout << menucnt << " – Выбрать сумму снятия наличных" << endl;
    cout << (menucnt + 1) << " – Назад" << endl;

    bool invalidcommand = 1;
    bool invalidsum = 1;

    while (invalidcommand) {
        invalidcommand = 1;
        try {
            cin >> summenuchoice; //Выбор между стандартными суммами выдачи, произвольной суммой, и возвращением в главное меню
        }
        catch (invalid_argument) {
            cout << "Неопознанная команда, повторите ввод" << endl;
            continue;
        }

        if ((menucnt > 1) and (1 <= summenuchoice) and (summenuchoice <= (menucnt - 1))) { //Выбор стандартной суммы
            finalsum = standardsum[summenuchoice - 1];
            invalidcommand = 0;
        }

        else if (summenuchoice <= (menucnt + 1)) { //Выбор пользовательской суммы
            invalidcommand = 0;
            if (summenuchoice == menucnt) {
                cout << "Введите желаемую сумму" << endl;
                while (invalidsum) {
                    invalidsum = 1;
                    try {
                        cin >> finalsum; //Ввод пользовательской суммы
                    }
                    catch (invalid_argument) {
                        cout << "Неопознанная команда, повторите ввод" << endl;
                        continue;
                    }
                    for (int i = 0; i < size(ConfigBills); i++) BillsBackup[i] = ConfigBills[i][1]; //Проверка возможности выдачи пользовательской суммы
                    customsumtest = LargeBills(finalsum);
                    for (int i = 0; i < size(ConfigBills); i++) ConfigBills[i][1] = BillsBackup[i];

                    if (customsumtest.empty()) {
                        cout << "Сумма не может быть выдана, повторите ввод" << endl;
                    }
                    else invalidsum = 0;
                }
            }
            else if (summenuchoice == (menucnt + 1)) return; //Возврат в главное меню
        }
        else continue;
    }
    system("cls");
    if (DLcnt > dailylimit) { //Проверка на достижение дневного лимита
        dailylimitreached = 1;
        return;
    }
    BillsMenu(finalsum); //Выбор формата выдачи нужной суммы
    return;
}


int main()
{
    setlocale(LC_ALL, "Russian");
    char mainmenuchoice;
    bool invalidcommand = 1;

    while (!endsession) {

        invalidcommand = 1;
        system("cls");


        if (dailylimitreached) {
            cout << "Превышен дневной лимит выдачи денег" << endl; //Прекращение работы банкомата при достижении дневного лимита
            return 0;
        }


        cout << "Доступные купюры:  "; //Вывод доступных купюр в банкомате
        for (int i = 0; i < size(ConfigBills); i++) {
            if (ConfigBills[i][1] > 0) cout << "  " << ConfigBills[i][0] << '(' << ConfigBills[i][1] << ')';
        }
        cout << endl;
        cout << "Баланс: " << UserBalance << " рублей" << endl; //Вывод баланса пользователя
        if (UserBalance < 0) {
            cout << "Обращаем Ваше внимание, что при несвоевременном погашении задолженности в отношении клиента будут приняты меры в соответствии с ФЗ №230 от 03.07.2016" << endl;
        } //Оповещение клиента о наличии задолженности

        cout << endl << endl;
        cout << "1 – Снять наличные" << endl;
        cout << "2 – Выход" << endl;

        while (invalidcommand) {
            invalidcommand = 1;
            cin >> mainmenuchoice;
            switch (mainmenuchoice) {
            case '1':
                invalidcommand = 0;
                system("cls");
                BLcnt = 0;
                SumMenu(); //Снятие наличных
                break;
            case '2':
                invalidcommand = 0;
                return 0; //Прекращение использования банкомата
                break;
            default:
                cout << "Неопознанная команда, повторите ввод" << endl;
                break;
            }
        }
    }
}
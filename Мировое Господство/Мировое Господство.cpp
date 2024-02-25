#include <stdio.h>
#include <tgbot/tgbot.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <windows.h>
#include <sstream>
#include <random>
#include <time.h>

using namespace std;
using namespace TgBot;

class Games {
public:
    ~Games() {
        for (int i = 0; i < 4; i++) {
            delete countries[i];
        }
    }
    bool isStart = 0;
    class Country {
    public:
        string name;
        double level=0;
        double ecology=0.9;
        vector <string> sanctions;
        bool isNuclear = 0;
        bool isEcology = 0;
        bool isSanctions = 0;
        bool isEnd = 0;
        bool isLive = 1;
        int cbombs = 0;
        int bombs = 0;
        int money = 1000;
        long long id;

        class City {
        public:
            string name;
            bool isLive = 1;
            bool isShield = 0;
            double evo, level, profit;
            bool isUsed = 0;
            bool isTotal = 0;
        };

        vector <City*> cities;

        Country& operator=(const Country& c) {
            this->name = c.name;
            this->level = c.level;
            this->isNuclear = c.isNuclear;
            this->bombs = c.bombs;
            this->money = c.money;
            this->id = c.id;
            this->ecology = c.ecology;
            this-> sanctions = c.sanctions;
            
            if (this->cities.size() == 0) {
                for (int i = 0; i < 3; i++) {
                    City* city = new City;
                    city->name = c.cities[i]->name;
                    city->evo = c.cities[i]->evo;
                    city->isLive = c.cities[i]->isLive;
                    city->isShield = c.cities[i]->isShield;
                    city->level = c.cities[i]->level;
                    city->profit = c.cities[i]->profit;
                    this->cities.push_back(city);
                }
            }
            else {
                for (int i = 0; i < 3; i++) {
                    delete this->cities[i];
                    City* city = new City;
                    city->name = c.cities[i]->name;
                    city->evo = c.cities[i]->evo;
                    city->isLive = c.cities[i]->isLive;
                    city->isShield = c.cities[i]->isShield;
                    city->level = c.cities[i]->level;
                    city->profit = c.cities[i]->profit;
                    city->isTotal = c.cities[i]->isTotal;
                    this->cities.push_back(city);
                }
            }
            return *this;
        }

        void create_city(string name, double evo) {
            City* city = new City;
            city->name = name;
            city->evo = evo;
            cities.push_back(city);
        }

        ~Country() {
            for (int i = cities.size() - 1; i >= 0; i--) {
                delete cities[i];
            }
        }

        void up_city(int num) {
            cities[num]->evo += 0.2;
            cities[num]->profit = cities[num]->evo * ecology * 300;
            cities[num]->isUsed = 1;
        }
        
        void attack(int num) {
            if (!cities[num]->isTotal) {
                if (cities[num]->isLive) {
                    if (cities[num]->isShield) {
                        cities[num]->isShield = 0;
                    }
                    else {
                        cities[num]->isLive = 0;
                        cities[num]->evo = 0;
                    }
                }
                else {
                    cities[num]->isTotal = 1;
                }
            }
        }
    };

    void balance(Bot& bot, Country* country, Message::Ptr& message) {
        bot.getApi().sendMessage(message->chat->id, u8"💰 Баланс Вашей страны: " + to_string(country->money) + "$");
    }

    void ecology(Bot& bot, Country* country, Message::Ptr& message){
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        if (country->money >= 200) {
            InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
            vector<InlineKeyboardButton::Ptr> buttons;

            InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
            new_btn->text = u8"Да";
            new_btn->callbackData = "ecology";
            buttons.push_back(new_btn);
            
            keyboard->inlineKeyboard.push_back(buttons);

            string answer = u8"Улучшаем экологию?";
            bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
            
        }
        else {
            bot.getApi().sendMessage(message->chat->id, u8"Недостаточно средств для улучшения экологии!");
        }
    }

    void shield(Bot& bot, Country* country, Message::Ptr& message) {
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        if (country->money >= 300) {
            InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
            vector<InlineKeyboardButton::Ptr> buttons;
            int isUsed = 0;
            for (int i = 0; i < 3; i++) {
                if (!country->cities[i]->isShield and country->cities[i]->isLive) {
                    InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                    new_btn->text = country->cities[i]->name;
                    new_btn->callbackData = "shield:" + to_string(i);
                    buttons.push_back(new_btn);
                }
                else {
                    isUsed++;
                }
            }
            if (isUsed == 3) {
                bot.getApi().sendMessage(message->chat->id, u8"На всех городах есть ядерный щит!");
            }
            else {
                keyboard->inlineKeyboard.push_back(buttons);

                string answer = u8"На какой город желаете наложить ядерный щит?";
                bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
            }
        }
        else {
            bot.getApi().sendMessage(message->chat->id, u8"Недостаточно средств для создания ядерного щита!");
        }
    }

    void update_city(Bot& bot, Country* country, Message::Ptr& message) {
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        if (country->money >= 150) {
            InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
            vector<InlineKeyboardButton::Ptr> buttons;
            int isUsed = 0;
            for (int i = 0; i < 3; i++) {
                if (!country->cities[i]->isUsed and country->cities[i]->isLive) {
                    InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                    new_btn->text = country->cities[i]->name;
                    new_btn->callbackData = "update_city:" + to_string(i);
                    buttons.push_back(new_btn);
                }
                else {
                    isUsed++;
                }
            }
            if (isUsed == 3) {
                bot.getApi().sendMessage(message->chat->id, u8"Все города улучшены!");
            }
            else {
                keyboard->inlineKeyboard.push_back(buttons);

                string answer = u8"Какой город желаете улучшить?";
                bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
            }
        }
        else {
            bot.getApi().sendMessage(message->chat->id, u8"Недостаточно средств для улучшения города!");
        }
    }

    void sanctions(Bot& bot, Country* country, Message::Ptr& message) {
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        if (!country->isSanctions) {
            InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
            vector<InlineKeyboardButton::Ptr> buttons;
            for (int i = 0; i < 4; i++) {
                if (countries[i] != country) {
                    InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                    new_btn->text = countries[i]->name;
                    new_btn->callbackData = "sanctions:" + to_string(i);
                    buttons.push_back(new_btn);
                }
            }
            keyboard->inlineKeyboard.push_back(buttons);
            string answer = u8"На какую страну желаете наложить санкции?";
            bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
        }
        else {
            string answer = u8"Вы уже наложили санкции!";
            bot.getApi().sendMessage(message->chat->id, answer);
        }
    }

    void bombs(Bot& bot, Country* country, Message::Ptr& message) {
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        if (country->isNuclear) {
            bot.getApi().sendMessage(message->chat->id, u8"Цена одной бомбы 350$\nВы можете создать максимум 2 бомбы за раунд");
            if (country->money >= 350) {
                if (country->cbombs < 2) {
                    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
                    vector<InlineKeyboardButton::Ptr> buttons;

                    InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                    new_btn->text = u8"Да";
                    new_btn->callbackData = "bombs";
                    buttons.push_back(new_btn);

                    keyboard->inlineKeyboard.push_back(buttons);

                    string answer = u8"Создать бомбу?";
                    bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
                }
                else {
                    bot.getApi().sendMessage(message->chat->id, u8"Вы создали максимальное количество бомб в этому раунде!");
                }
            }
            else {
                bot.getApi().sendMessage(message->chat->id, u8"Недостаточно средств для создания бомбы!");
            }
        }
        else {
            bot.getApi().sendMessage(message->chat->id, u8"Для создания бомб Вам требуется разработать ядерную программу за 500$\nЭто также приведёт к спаду экологии в мире на 3%");
            if (country->money >= 500) {
                if (country->cbombs == 0) {
                    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
                    vector<InlineKeyboardButton::Ptr> buttons;

                    InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                    new_btn->text = u8"Да";
                    new_btn->callbackData = "nuclear";
                    buttons.push_back(new_btn);

                    keyboard->inlineKeyboard.push_back(buttons);

                    string answer = u8"Разрабатываем ядерную программу?";
                    bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
                }
                else {
                    bot.getApi().sendMessage(message->chat->id, u8"Вы уже начали разрабатывать ядерную программу!");
                }
            }
            else {
                bot.getApi().sendMessage(message->chat->id, u8"Недостаточно средств для разработки ядерной программы!");
            }
        }
    }

    void attack(Bot &bot, Country *country, Message::Ptr &message) {
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        if (country->bombs == 0) {
            bot.getApi().sendMessage(message->chat->id, u8"У вас нет бомб!");
            return;
        }
        InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
        vector<InlineKeyboardButton::Ptr> buttons;
        for (int i = 0; i < 4; i++) {
            if (countries[i] != country) {
                InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                new_btn->text = countries[i]->name;
                new_btn->callbackData = "attack:" + to_string(i);
                buttons.push_back(new_btn);
            }
        }
        keyboard->inlineKeyboard.push_back(buttons);
        string answer = u8"Какую страну желаете атаковать?";
        bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
    }

    void attack_city(Bot& bot, Country* country, TgBot::CallbackQuery::Ptr &query, int count) {
        if (country->isEnd) {
            bot.getApi().sendMessage(query->message->chat->id, u8"Вы уже закончили ход!");
            return;
        }

        InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
        vector<InlineKeyboardButton::Ptr> buttons;
        int isLive = 0;
        for (int i = 0; i < 3; i++) {
            if (countries[count]->cities[i]->isLive) {
                InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
                new_btn->text = countries[count]->cities[i]->name;
                new_btn->callbackData = "attack_city:" + to_string(count) + ":" + to_string(i);

                buttons.push_back(new_btn);
            }
            else {
                isLive++;
            }
        }
        if (isLive != 3) {
            keyboard->inlineKeyboard.push_back(buttons);

            string answer = u8"Какой город?";
            bot.getApi().editMessageText(answer, query->message->chat->id, query->message->messageId, "", "", false, keyboard);
        }
        else {
            bot.getApi().editMessageText(u8"Все города этой страны учничтожены", query->message->chat->id, query->message->messageId);
        }
    }

    void help(Bot& bot, Message::Ptr& message) {
        string answer = u8"<b>Команды:</b>\n\n";
        answer += u8"🏙 <b>Информация о стране</b> — Получить подробную информацию о вашей стране и городах\n\n";

        answer += u8"🌎 <b>Информация о мире</b> — Получить информацию о всех странах и экологии\n<i>Вы увидите только уровень жизни в каждом городе вражеских стран. Информация о присутствии ядерного щита на городах вражеских стран не отображается</i>\n\n";

        answer += u8"💰 <b>Баланс</b> — Получить информацию о балансе средств вашей страны\n\n";

        answer += u8"⏫ <b>Улучшить город</b> — Вы улучшите развитие своего города на 20%, что даст вам прирост прибыли\n<i>Прибыль, получаемая каждый раунд, складывается из прибыли каждого вашего города. На их прибыль влияет уровень жизни (зависит от развитости и экологии в мире)</i>\n\n";

        answer += u8"🛡 <b>Установить ядерный щит</b> — Вы установите ядерный щит, который защитит ваш город в случае попадания по нему бомбы\n<i>На каждом городе может быть всего один щит. Если он спадёт, вы можете установить его заново\nЕсли на городе установлен щит и на него сбросили бомбу, то щит спадёт и при этом город останется цел. Однако повторный удар по вашему городу станет фатальным</i>\n\n";

        answer += u8"💣 <b>Ядерное оружие</b> — Разработать ядерную программу/Создать ядерную бомбу\nДля того, чтобы создавать ядерное оружие вам надо разработать ядерную программу. После этого со следующего раунда вы сможете создавать бомбы\n<i>За раунд вы можете создать максимум 2 ядерных бомбы</i>\n\n";

        answer += u8"♻️ <b>Улучшить экологию в мире</b> — Уровень экологии в мире поднимется на 20%\n<i>У уровня экологии нет предела. Он может быть и выше 100%</i>\n\n";

        answer += u8"📄 <b>Наложить санкции</b> — В стране, на которую вы наложите санкции, снизится прибыль на 10%\n<i>Страна увидит, что санкции наложили именно вы</i>\n\n";

        answer += u8"🚀 <b>Нанести ядерный удар</b> — Сбросить ядерную бомбу на город другой страны\nЗа раунд можно сбрасывать любое количество ядерных бомб на любые вражеские города\n<i>За каждый уничтоженный город экология в мире снижается на 12%</i>\n\n";

        answer += u8"✅ <b>Закончить ход</b> — Вы закончите свой ход\nКогда все игроки закончат ход, наступит следующий раунд\n<i>Если вы закончите ход, вы больше не сможете управлять страной</i>\n\n";

        answer += u8"ℹ️ <b>Помощь</b> — Получить список команд и информацию о них\n\n";
        bot.getApi().sendMessage(message->chat->id, answer, false, 0, TgBot::GenericReply::Ptr(), "HTML");
    }

    void command_help(Bot& bot, Message::Ptr& message) {
        string answer = u8"Команды:\n\n";
        answer += u8"/help - Получить список команд и информацию о них\n\n";
        answer += u8"/world - Получить информацию о всех странах\n\n";
        answer += u8"/info - Получить информацию о Вашей стране\n\n";
        answer += u8"/balance - Получить информацию о балансе средств Вашей страны\n\n";
        answer += u8"/update - Улучшить Ваш город\n\n";
        answer += u8"/ecology - Улучшить экологию во всём мире\n\n";
        answer += u8"/bombs - Разработать ядерную программу/Создать ядерную бомбу\n\n";
        answer += u8"/shield - Установить ядерный щит, который защитит ваш город\n\n";
        answer += u8"/sanctions - Наложить санкции на другую страну\n\n";
        answer += u8"/attack - Сбросить ядерную бомбу на город другой страны\n\n";
        answer += u8"/end - Закончить свой ход\n\n";
        bot.getApi().sendMessage(message->chat->id, answer);
    }

    void menu(Bot& bot, Message::Ptr& message) {
        TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
        vector<TgBot::KeyboardButton::Ptr> row0;

        // Создаем кнопку для команды "/command1"
        TgBot::KeyboardButton::Ptr button1(new TgBot::KeyboardButton);
        button1->text = u8"🏙 Информация о стране";
        row0.push_back(button1);

        // Создаем кнопку для команды "/command2"
        TgBot::KeyboardButton::Ptr button2(new TgBot::KeyboardButton);
        button2->text = u8"🌎 Информация о мире";
        row0.push_back(button2);

        TgBot::KeyboardButton::Ptr button3(new TgBot::KeyboardButton);
        button3->text = u8"💰 Баланс";
        row0.push_back(button3);

        keyboard->keyboard.push_back(row0);

        vector<TgBot::KeyboardButton::Ptr> row1;

        // Создаем кнопку для команды "/command1"
        TgBot::KeyboardButton::Ptr button4(new TgBot::KeyboardButton);
        button4->text = u8"⏫ Улучшить город";
        row1.push_back(button4);

        // Создаем кнопку для команды "/command2"
        TgBot::KeyboardButton::Ptr button5(new TgBot::KeyboardButton);
        button5->text = u8"🛡 Установить ядерный щит";
        row1.push_back(button5);

        TgBot::KeyboardButton::Ptr button6(new TgBot::KeyboardButton);
        button6->text = u8"💣 Ядерное оружие";
        row1.push_back(button6);

        keyboard->keyboard.push_back(row1);

        vector<TgBot::KeyboardButton::Ptr> row2;

        // Создаем кнопку для команды "/command1"
        TgBot::KeyboardButton::Ptr button7(new TgBot::KeyboardButton);
        button7->text = u8"♻️ Улучшить экологию в мире";
        row2.push_back(button7);

        // Создаем кнопку для команды "/command2"
        TgBot::KeyboardButton::Ptr button8(new TgBot::KeyboardButton);
        button8->text = u8"📄 Наложить санкции";
        row2.push_back(button8);

        TgBot::KeyboardButton::Ptr button9(new TgBot::KeyboardButton);
        button9->text = u8"🚀 Нанести ядерный удар";
        row2.push_back(button9);

        keyboard->keyboard.push_back(row2);

        vector<TgBot::KeyboardButton::Ptr> row3;

        // Создаем кнопку для команды "/command1"
        TgBot::KeyboardButton::Ptr button10(new TgBot::KeyboardButton);
        button10->text = u8"✅ Закончить ход";
        row3.push_back(button10);

        // Создаем кнопку для команды "/command2"
        TgBot::KeyboardButton::Ptr button11(new TgBot::KeyboardButton);
        button11->text = u8"ℹ️ Помощь";
        row3.push_back(button11);

        keyboard->keyboard.push_back(row3);

        // Отправляем сообщение с клавиатурой
        bot.getApi().sendMessage(message->chat->id, u8"Выберите команду:", false, 0, keyboard);
    }

    void world(Bot& bot, Message::Ptr& message) {
        world(bot, message->chat->id);
    }

    void info(Bot& bot, Country* country, Message::Ptr& message) {
        info(bot, country, message->chat->id);
    }

    void world(Bot& bot, long long id) {
        string answer = "";
        answer += u8"♻️ *Экология в мире: " + to_string(static_cast<int>(countries[0]->ecology * 100)) + "%*\n";

        for (int k = 0; k < 4; k++) {
            answer += "\n*" + countries[k]->name + "*\n";
            for (int i = 0; i < 3; i++) {
                if (countries[k]->cities[i]->evo != 0) {
                    answer += countries[k]->cities[i]->name + " " + to_string(static_cast<int>(countries[k]->cities[i]->evo * countries[k]->ecology * 100)) + "%\n";
                }
                else {
                    answer += u8"~" + countries[k]->cities[i]->name + "~\n";
                }
            }
            answer += u8"_Среднее " + to_string(static_cast<int>(countries[k]->level * 100)) + u8"%_\n";
        }
        bot.getApi().sendMessage(id, answer, false, 0, TgBot::GenericReply::Ptr(), "MarkdownV2");
    }

    void info(Bot& bot, Country* country, long long id) {
        string answer = u8"*Вы играете за " + country->name;
        answer += u8"*\n*Ваши города:*\n";
        for (int i = 0; i < 3; i++) {
            if (country->cities[i]->evo != 0) {
                if (country->cities[i]->isShield) {
                    answer += u8"🏙️ *" + country->cities[i]->name + u8"* 🛡\n";
                }
                else {
                    answer += u8"🏙️ *" + country->cities[i]->name + "*\n";
                }
                answer += u8"Развитость " + to_string(static_cast<int>(country->cities[i]->evo * 100)) + "%\n";
                answer += u8"Уровень жизни " + to_string(static_cast<int>(country->cities[i]->evo * country->ecology * 100)) + "%\n";
                answer += u8"Прибыль " + to_string(static_cast<int>(country->cities[i]->evo * country->ecology * 300)) + "$\n\n";
            }
            else {
                answer += u8"💢 ~" + country->cities[i]->name + "~\n\n";
            }
        }
        answer += u8"💰 Всего средств: " + to_string(country->money) + "$\n";
        answer += u8"💣 Бомб имеется: " + to_string(country->bombs) + "\n";
        answer += u8"📄 Санкции: ";
        for (int i = 0; i < country->sanctions.size(); i++) {
            answer += country->sanctions[i] + " ";
        }
        //cout << answer;
        bot.getApi().sendMessage(id, answer, false, 0, TgBot::GenericReply::Ptr(), "MarkdownV2");
    }

    void full_info(Bot& bot, Country* country, Country* newCountry, Message::Ptr& message) {
        string answer = "";
        answer += country->name + " " + to_string(reinterpret_cast<uintptr_t>(country)) + "\n";
        for (int i = 0; i < 3; i++) {
            answer += country->cities[i]->name + " " + to_string(static_cast<int>(country->cities[i]->evo*100)) + "%\n";
        }
        if (country->sanctions.size() != 0) {
            answer += u8"Санкции: ";
            for (int i = 0; i < country->sanctions.size(); i++) {
                answer += country->sanctions[i] + " ";
            }
            answer += "\n";
        }
        answer += u8"Бомбы: " + to_string(country->bombs);
        answer += to_string(country->money) + "\n\n";
        answer += newCountry->name + " " + to_string(reinterpret_cast<uintptr_t>(newCountry)) + "\n";
        
        
        for (int i = 0; i < 3; i++) {
            answer += newCountry->cities[i]->name + " " + to_string(static_cast<int>(newCountry->cities[i]->evo*100)) + "%\n"; //to_string(reinterpret_cast<uintptr_t>(newCountry->cities[i]))
        }
        if (newCountry->sanctions.size() != 0) {
            answer += u8"Санкции: ";
            for (int i = 0; i < newCountry->sanctions.size(); i++) {
                answer += newCountry->sanctions[i] + " ";
            }
            answer += "\n";
        }
        answer += u8"Бомбы: " + to_string(newCountry->bombs);
        answer += to_string(newCountry->money) + "\n\n";
        bot.getApi().sendMessage(message->chat->id, answer);
    }

    void end(Bot& bot, Country* country, Message::Ptr& message) {
        if (country->isEnd) {
            bot.getApi().sendMessage(message->chat->id, u8"Вы уже закончили ход!");
            return;
        }
        InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
        vector<InlineKeyboardButton::Ptr> buttons;

        InlineKeyboardButton::Ptr new_btn(new InlineKeyboardButton);
        new_btn->text = u8"Да";
        new_btn->callbackData = "end";
        buttons.push_back(new_btn);

        keyboard->inlineKeyboard.push_back(buttons);

        string answer = u8"Закончить ход?";
        bot.getApi().sendMessage(message->chat->id, answer, false, 0, keyboard);
    }

    Country* countries[4];
    Country* newCountries[4];
    Country* us, *ru, *ge, *ch;
    vector <long long> players;

    int players_num = 4;

    void next_turn(Country* countries[], Country* newCountries[], unordered_map<long long, Country*> &countryMap, unordered_map<long long, Country*> &newCountryMap, Bot &bot) {
        if (countries[0]->ecology <= 0) {
            for (int i = 0; i < players.size(); i++) {
                string answer = u8"☢️ Игра окончена. Экология упала до нуля ☢️";
                isStart = 0;
                bot.getApi().sendMessage(players[i], answer);
            }
            for (int i = 0; i < 4; i++) {
                countries[i]->ecology = 0;
            }
        }
        for (int i = 0; i < 4; i++) {
            delete countries[i];
            Country* newCountry = new Country;
            *newCountry = *newCountries[i];
            countries[i] = newCountry;

            double level = 0;
            for (int k = 0; k < 3; k++) {
                countries[i]->cities[k]->profit = countries[i]->cities[k]->evo * 300 * countries[i]->ecology;
                countries[i]->money += (countries[i]->cities[k]->profit)*(1 - 0.1*countries[i]->sanctions.size());
                level += countries[i]->cities[k]->evo * countries[i]->ecology;
            }
            countries[i]->level = level/3;
            delete newCountries[i];
            Country* newCountryy = new Country;
            *newCountryy = *countries[i];

            newCountries[i] = newCountryy;
            newCountries[i]->sanctions.clear();
            for (int i = 0; i < players_num; i++) {
                countryMap[players[i]] = countries[i];
                newCountryMap[players[i]] = newCountries[i];
            }
        }
    }

    void rand_play(unordered_map<long long, Country*> &countryMap, unordered_map<long long, Country*>& newCountryMap) {
        vector <long long> tmp = players;
        players.clear();
        for (int i = 0; i < players_num; i++) {
            int a = rand() % tmp.size();
            countryMap[tmp[a]] = countries[i];
            newCountryMap[tmp[a]] = newCountries[i];
            players.push_back(tmp[a]);
            tmp.erase(tmp.begin() + a);
        }
    }

    void start_game() {
        for (int i = 0; i < 4; i++) {
            countries[i] = new Country;
        }
        create_countries(countries);
        for (int i = 0; i < 4; i++) {
            Country *newCountry = new Country;
            *newCountry = *countries[i];
            newCountries[i] = newCountry;
        }

        unordered_map<long long, Country*> countryMap;
        unordered_map<long long, Country*> newCountryMap;

        int count=0;

        Bot bot("####");

        bot.getEvents().onCommand("help", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            help(bot, message);
        });
        bot.getEvents().onCommand("full_info", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            full_info(bot, countryMap[message->chat->id], newCountryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("world", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            world(bot, message);
        });
        bot.getEvents().onCommand("end", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            end(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("bombs", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            bombs(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("sanctions", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            sanctions(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("info", [&bot, &countryMap, &newCountryMap, this](Message::Ptr message) {
            info(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("balance", [&bot, &countryMap, this](Message::Ptr message) {
            //countryMap[message->chat->id]->up_city(bot);
            balance(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("update", [&bot, &countryMap, this](Message::Ptr message){
            //countryMap[message->chat->id]->up_city(bot);
            bot.getApi().sendMessage(message->chat->id, u8"Улучшение города стоит 150$");
            update_city(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("shield", [&bot, &countryMap, this](Message::Ptr message) {
            //countryMap[message->chat->id]->up_city(bot);
            bot.getApi().sendMessage(message->chat->id, u8"Постройка ядерного щита стоит 300$");
            shield(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("attack", [&bot, &countryMap, this](Message::Ptr message) {
            //countryMap[message->chat->id]->aaattack(bot, *this, message);
            //countryMap[message->chat->id]->check(bot, message);
            attack(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCommand("ecology", [&bot, &countryMap, this](Message::Ptr message) {
            bot.getApi().sendMessage(message->chat->id, u8"Улучшение экологии стоит 200$");
            ecology(bot, countryMap[message->chat->id], message);
        });
        bot.getEvents().onCallbackQuery([&bot, this, &countryMap, &newCountryMap, &count](TgBot::CallbackQuery::Ptr query) {
            stringstream ss(query->data);
            string token;
            vector <string> words;
            while (getline(ss, token, ':')) {
                words.push_back(token);
            }
            if (words[0] == "attack") {
                bot.getApi().answerCallbackQuery(query->id, u8"Атака на "+ countries[stoi(words[1])]->name);
                attack_city(bot, countryMap[query->message->chat->id], query, stoi(words[1]));
            }
            else if (words[0] == "attack_city") {
                bot.getApi().answerCallbackQuery(query->id, u8"Атака на " + countries[stoi(words[1])]->cities[stoi(words[2])]->name);
                bot.getApi().editMessageText(u8"Ядерная бомба скинута на город " + countries[stoi(words[1])]->cities[stoi(words[2])]->name, query->message->chat->id, query->message->messageId);
                newCountries[stoi(words[1])]->attack(stoi(words[2]));
                if (!newCountries[stoi(words[1])]->cities[stoi(words[2])]->isLive and !newCountries[stoi(words[1])]->cities[stoi(words[2])]->isTotal) {
                    for (int i = 0; i < 4; i++) {
                        newCountries[i]->ecology -= 0.12;
                    }
                }
                countryMap[query->message->chat->id]->bombs -= 1;
                newCountryMap[query->message->chat->id]->bombs -= 1;
                bot.getApi().sendMessage(query->message->chat->id, u8"Осталось ядерных бомб: "+ to_string(countryMap[query->message->chat->id]->bombs));
            }
            else if (words[0] == "update_city") {
                bot.getApi().answerCallbackQuery(query->id, u8"Улучшение");
                bot.getApi().editMessageText(u8"Улучшен город " + countryMap[query->message->chat->id]->cities[stoi(words[1])]->name, query->message->chat->id, query->message->messageId);
                countryMap[query->message->chat->id]->up_city(stoi(words[1]));
                if (newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->isLive) {
                    newCountryMap[query->message->chat->id]->up_city(stoi(words[1]));
                }
                countryMap[query->message->chat->id]->money -= 150;
                newCountryMap[query->message->chat->id]->money -= 150;
                balance(bot, countryMap[query->message->chat->id], query->message);
            }
            else if (words[0] == "ecology") {
                bot.getApi().answerCallbackQuery(query->id, u8"Экология");
                bot.getApi().editMessageText(u8"Экология будет улучшена!", query->message->chat->id, query->message->messageId);
                countryMap[query->message->chat->id]->money -= 200;
                newCountryMap[query->message->chat->id]->money -= 200;
                balance(bot, countryMap[query->message->chat->id], query->message);
                for (int i = 0; i < 4; i++) {
                    newCountries[i]->ecology += 0.2;
                }
            }
            else if (words[0] == "shield") {
                bot.getApi().answerCallbackQuery(query->id, u8"Ядерный щит");
                bot.getApi().editMessageText(u8"Ядерный щит установлен на город " + countryMap[query->message->chat->id]->cities[stoi(words[1])]->name, query->message->chat->id, query->message->messageId);
                countryMap[query->message->chat->id]->money -= 300;
                newCountryMap[query->message->chat->id]->money -= 300;
                balance(bot, countryMap[query->message->chat->id], query->message);
                if (newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->isLive) {
                    countryMap[query->message->chat->id]->cities[stoi(words[1])]->isShield = 1;
                    newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->isShield = 1;
                }
                else if (!newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->isTotal){
                    countryMap[query->message->chat->id]->cities[stoi(words[1])]->isShield = 1;
                    newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->isShield = 0;
                    newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->isLive = 1;
                    newCountryMap[query->message->chat->id]->cities[stoi(words[1])]->evo = countryMap[query->message->chat->id]->cities[stoi(words[1])]->evo;
                    for (int i = 0; i < 4; i++) {
                        newCountries[i]->ecology += 0.12;
                    }
                }
            }
            else if (words[0] == "sanctions") {
                bot.getApi().answerCallbackQuery(query->id, u8"Санкции");
                bot.getApi().editMessageText(u8"Вы наложили санкции на страну " + countries[stoi(words[1])]->name, query->message->chat->id, query->message->messageId);
                countryMap[query->message->chat->id]->isSanctions = 1;
                newCountries[stoi(words[1])]->sanctions.push_back(countryMap[query->message->chat->id]->name);
            }
            else if (words[0] == "nuclear") {
                bot.getApi().answerCallbackQuery(query->id, u8"Ядерная программа");
                bot.getApi().editMessageText(u8"Вы успешно начали разработку ядерной программы!", query->message->chat->id, query->message->messageId);
                countryMap[query->message->chat->id]->money -= 500;
                newCountryMap[query->message->chat->id]->money -= 500;
                balance(bot, countryMap[query->message->chat->id], query->message);
                countryMap[query->message->chat->id]->cbombs = 1;
                newCountryMap[query->message->chat->id]->isNuclear = 1;
                for (int i = 0; i < 4; i++) {
                    newCountries[i]->ecology -= 0.03;
                }
            }
            else if (words[0] == "bombs") {
                bot.getApi().answerCallbackQuery(query->id, u8"Бомба");
                countryMap[query->message->chat->id]->money -= 350;
                newCountryMap[query->message->chat->id]->money -= 350;
                balance(bot, countryMap[query->message->chat->id], query->message);
                countryMap[query->message->chat->id]->cbombs += 1;
                newCountryMap[query->message->chat->id]->bombs += 1;
                bot.getApi().editMessageText(u8"Вы создали ядерную бомбу!\nВ следующем раунде у вас будет ядерных бомб: " + to_string(newCountryMap[query->message->chat->id]->bombs), query->message->chat->id, query->message->messageId);
            }
            else if (words[0] == "end") {
                bot.getApi().answerCallbackQuery(query->id, u8"Конец хода");
                bot.getApi().editMessageText(u8"Вы закончили ход!", query->message->chat->id, query->message->messageId);
                countryMap[query->message->chat->id]->isEnd = 1;
                count++;

                string answer = u8"❗️ " + to_string(count) + u8" из 4 стран закончили ход ❗️";
                for (int i = 0; i < players.size(); i++) {
                    bot.getApi().sendMessage(players[i], answer);
                }

                if (count == players_num) {
                    string answer = u8"Новый раунд начался";
                    for (int i = 0; i < players.size(); i++) {
                        bot.getApi().sendMessage(players[i], answer);
                    }
                    next_turn(countries, newCountries, countryMap, newCountryMap, bot);
                    for (int i = 0; i < players.size(); i++) {
                        cout << players[0] << " " << players[1] << '\n';
                        world(bot, players[i]);
                        info(bot, countryMap[players[i]], players[i]);
                    }
                    count = 0;
                }
            }

        });

        bot.getEvents().onCommand("menu", [&bot, this](TgBot::Message::Ptr message) {
            menu(bot, message);
        });
        bot.getEvents().onCommand("start", [&bot, this, &count, &countryMap, &newCountryMap](TgBot::Message::Ptr message) {
            if (isStart) {
                return;
            }
            bot.getApi().sendMessage(message->chat->id, u8"Вы успешно зарегистрировались в игре");
            players.push_back(message->chat->id);
            count++;
            if (count == players_num) {
                count = 0;
                rand_play(countryMap, newCountryMap);
                for (int i = 0; i < players.size(); i++) {
                    cout << players[0] << " " << players[1] << '\n';
                    world(bot, players[i]);
                    info(bot, countryMap[players[i]], players[i]);
                }
                isStart = 1;
            }
        });

        bot.getEvents().onAnyMessage([&bot, &countryMap, this](Message::Ptr message) {
            if (!isStart && message->text!="/start") {
                bot.getApi().sendMessage(message->chat->id, u8"Игра ещё не началась!");
                return;
            }
            printf("User wrote %s\n", message->text.c_str());
            if (message->text == u8"⏫ Улучшить город") {
                bot.getApi().sendMessage(message->chat->id, u8"Улучшение города стоит 150$");
                update_city(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"🏙 Информация о стране") {
                info(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"🌎 Информация о мире") {
                world(bot, message);
            }
            if (message->text == u8"💰 Баланс") {
                balance(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"🛡 Установить ядерный щит") {
                bot.getApi().sendMessage(message->chat->id, u8"Постройка ядерного щита стоит 300$");
                shield(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"💣 Ядерное оружие") {
                bombs(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"♻️ Улучшить экологию в мире") {
                bot.getApi().sendMessage(message->chat->id, u8"Улучшение экологии стоит 200$");
                ecology(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"📄 Наложить санкции") {
                sanctions(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"🚀 Нанести ядерный удар") {
                attack(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"✅ Закончить ход") {
                end(bot, countryMap[message->chat->id], message);
            }
            if (message->text == u8"ℹ️ Помощь") {
                help(bot, message);
            }
        });
        try {
            printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
            TgLongPoll longPoll(bot,100,30000);
            while (true) {
                printf("Long poll started\n");
                longPoll.start();
            }
        }
        catch (TgException& e) {
            printf("error: %s\n", e.what());
        }
       
    }

    void create_countries(Country* countries[]) {
        Country* us=countries[0], *ru= countries[1], *ge= countries[2], *ch= countries[3];
    
        us->name = u8"🇺🇸 США";
        us->create_city(u8"Вашингтон", 0.54);
        us->create_city(u8"Нью—Йорк", 0.54);
        us->create_city(u8"Чикаго", 0.54);
        for (int i = 0; i < 3; i++) {
            us->level += us->cities[i]->evo*us->ecology;
        }
        us->level /= 3;

        ru->name = u8"🇷🇺 Россия";
        ru->create_city(u8"Москва", 0.9);
        ru->create_city(u8"Санкт—Петербург", 0.36);
        ru->create_city(u8"Новосибирск", 0.36);
        for (int i = 0; i < 3; i++) {
            ru->level += ru->cities[i]->evo * ru->ecology;
        }
        ru->level /= 3;

        ge->name = u8"🇩🇪 Германия";
        ge->create_city(u8"Берлин", 0.72);
        ge->create_city(u8"Мюнхен", 0.45);
        ge->create_city(u8"Гамбург", 0.45);
        for (int i = 0; i < 3; i++) {
            ge->level += ge->cities[i]->evo * ge->ecology;
        }
        ge->level /= 3;

        ch->name = u8"🇨🇳 Китай";
        ch->create_city(u8"Пекин", 0.36);
        ch->create_city(u8"Шанхай", 0.9);
        ch->create_city(u8"Гонконг", 0.36);
        for (int i = 0; i < 3; i++) {
            ch->level += ch->cities[i]->evo * ch->ecology;
        }
        ch->level /= 3;

        cout << "США:\n";
        for (int i = 0; i < us->cities.size(); i++) {
            cout << us->cities[i]->name << "\t";
        }
        cout << "\n\n";

        cout << "Россия:\n";
        for (int i = 0; i < us->cities.size(); i++) {
            cout << ru->cities[i]->name << "\t";
        }
        cout << "\n\n";

        cout << "Германия:\n";
        for (int i = 0; i < us->cities.size(); i++) {
            cout << ge->cities[i]->name << "\t";
        }
        cout << "\n\n";

        cout << "Китай:\n";
        for (int i = 0; i < us->cities.size(); i++) {
            cout << ch->cities[i]->name << "\t";
        }
        cout << "\n\n";
    }

};

int main() {
    setlocale(LC_ALL, "Ru");
    srand(time(NULL));
    Games game;
    game.start_game();

    vector <long long> players({ 1, 2, 3, 4 });
    vector <long long> tmp = players;
    players.clear();
    for (int i = 0; i < 4; i++) {
        int a = rand() % tmp.size();
        cout << tmp[a] << " ";
        players.push_back(tmp[a]);
        tmp.erase(tmp.begin() + a);
    }
    cout << "\n";
    for (int i = 0; i < 4; i++) {
        cout << players[i] << " ";
    }
    
}
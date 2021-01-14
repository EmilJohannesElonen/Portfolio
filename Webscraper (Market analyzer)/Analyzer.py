# Author: Emil Elonen, emil.elonen@hotmail.com
# Last update: 14.01.2021
#
# This is a script to analyze the market history of the Helsinki Stock
# Exchange. The database has to be created with Webscraper.py and must be valid
# for the script to work as expected. The analyzing can be done for each month
# and the results show which stocks have performed the best on average during
# that month.

import os


def init_db():

    files = []
    result = {"01": {}, "02": {}, "03": {}, "04": {}, "05": {},
              "06": {}, "07": {}, "08": {}, "09": {}, "10": {},
              "11": {}, "12": {}}

    basepath = "database/"
    for file in os.listdir(basepath):
        if os.path.isfile(os.path.join(basepath, file)):
            files.append(file)

    i = 0
    for file in files:
        ticker = str(file).split(".")[0].strip()

        datastream = open(basepath + str(file), "r")

        ticker_data = {}

        first_year = 2020
        last_year = 0

        for row in datastream:
            if len(row.strip()) < 1:  # empty line
                pass
            else:
                timestamp = row.split(":")[1].strip().split(" ")[0].strip()
                year = timestamp[6]+timestamp[7]+timestamp[8]+timestamp[9]
                month = timestamp[3]+timestamp[4]
                day = timestamp[0]+timestamp[1]

                if int(year) < first_year:
                    first_year = int(year)
                if int(year) > last_year:
                    last_year = int(year)

                if row.split(":")[2].strip() == "No data avialable":
                    prize = -0.1
                else:
                    prize = float(row.split(":")[2].strip().replace(",", "."))

                ticker_data[year+month+day] = prize

        y = first_year
        while y <= last_year:
            m = 1
            while m <= 12:
                first_day = 0
                last_day = 0
                for data in ticker_data:
                    if int(data[0]+data[1]+data[2]+data[3]) == y:
                        if int(data[4]+data[5]) == m:
                            if ticker_data[data] != -0.1:
                                if first_day == 0:
                                    first_day = int(data[6]+data[7])
                                    last_day = int(data[6]+data[7])
                                else:
                                    if int(data[6]+data[7]) < first_day:
                                        first_day = int(data[6]+data[7])
                                    if int(data[6]+data[7]) > last_day:
                                        last_day = int(data[6]+data[7])

                # Check that month data is valid (most of month is recorded)
                if last_day > 24 and first_day < 5:
                    if m < 10:
                        tmstmp = str(y)+"0"+str(m)
                        mon = "0"+str(m)
                    else:
                        tmstmp = str(y)+str(m)
                        mon = str(m)
                    begin = ticker_data[tmstmp+"0"+str(first_day)]
                    finish = ticker_data[tmstmp+str(last_day)]
                    change = ((finish-begin)/begin)*100  # in percent

                    if ticker not in result[mon]:
                        result[mon][ticker] = {}

                    result[mon][ticker][str(y)] = change
                    print("Initialising database. Initialized " + str(i) +
                          " tickers.")

                m += 1
            y += 1
        i += 1

    print("Initialising database complete. Initialised " + str(i) +
          " tickers in total.")

    return result


def analyze_month(month, db):

    avgs = {}

    if month < 10:
        info = db["0"+str(month)]
    else:
        info = db[str(month)]

    for ticker in info:
        i = 0
        sum_ = 0
        for year in info[ticker]:
            sum_ += info[ticker][year]
            i += 1
        avg = sum_/i
        avgs[ticker] = avg

    topten = []

    for tic in avgs:
        if len(topten) == 0:
            topten = [tic]
        else:
            found = 0
            for a in topten:
                if avgs[tic] > avgs[a]:
                    found = 1
                    topten.insert(topten.index(a), tic)
                    if len(topten) > 10:
                        del topten[-1]
                    break
            if found == 0:
                if len(topten) < 10:
                    topten.insert(len(topten), tic)

    b = 1
    for winner in topten:
        print(str(b)+". "+winner+"    "+"{:.2f}".format(avgs[winner]))

        for y in info[winner]:
            print(str(y) + " : " + "{:.2f}".format(info[winner][y]),
                  end="    ")

        print()
        print()
        b += 1


def main():
    db = init_db()

    while True:
        cmd = input("Enter the month to analyze (01, 02, 03, etc...): ")

        if cmd.isnumeric() and 0 < int(cmd) < 13:
            analyze_month(int(cmd), db)

        elif cmd == "Q" or cmd == "q":
            print("Goodbye and good luck with your investments! Remember that "
                  "I am just a bot and can in no way guarantee any success on "
                  "the market :)")
            break

        else:
            print("Unknown command")


if __name__ == "__main__":
    main()

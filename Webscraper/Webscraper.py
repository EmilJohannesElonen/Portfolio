# Author: Emil Elonen, emil.elonen@hotmail.com
# Last update: 14.01.2021
#
# This script uses Selenium to webscrape market history data from
# Kauppalehti.fi. It creates a database for all the tickers in the reference
# file with history data from max. 10 years. If database exist already, the
# only missing information is added.

import time
import urllib.request
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import os.path


def main():

    tickers = []
    file = open("HEL_yhtiöt_lyhenteillä.txt", "r")
    for row in file:
        ticker = row.split("/")[1].strip()
        tickers.append(ticker)
    file.close()

    for TICKER in tickers:

        data = {}
        timestamps = []

        file_path = "database/"+TICKER+".txt"
        data_file = open(file_path, "r")

        for row in data_file:
            if row.split(":").__len__() > 1:
                pvm = row.split(":")[1].strip().split(" ")[0].strip()
                prize = row.split(":")[2].strip()

                year = pvm.split(".")[2]
                month = pvm.split(".")[1]
                day = pvm.split(".")[0]

                ts = year+month+day

                timestamps.append(ts)
                data[pvm] = prize

        data_file.close()

        driver = webdriver.Chrome("chromedriver")

        driver.get("https://www.kauppalehti.fi/porssi/porssikurssit/osake/"+TICKER+"/kurssihistoria")  # put here the adress of your page
        elem = driver.find_elements_by_xpath('/html/body/div[2]/div[6]/div[3]/div[2]/div[3]/div[1]/button[10]')  # put here the content you have put in Notepad, ie the XPath
        driver.execute_script("arguments[0].click();", elem[0])  # Setting period to 10 years
        print(elem[0].get_attribute('class'))

        time.sleep(1)

        i = 1
        while True:
            try:
                xpath = "/html/body/div[2]/div[6]/div[3]/div[2]/div[4]/div[2]/div/div[2]/div/div["+str(i)+"]/div/div[1]/div/h5/span/time"
                element = driver.find_elements_by_xpath(xpath)
                date = element[0].__getattribute__('text')
                if date not in data:
                    year = date.split(".")[2]
                    month = date.split(".")[1]
                    day = date.split(".")[0]

                    ts = year + month + day

                    timestamps.append(ts)

                    try:
                        xpath = "/html/body/div[2]/div[6]/div[3]/div[2]/div[4]/div[2]/div/div[2]/div/div["+str(i)+"]/div/div[4]/span"
                        element = driver.find_elements_by_xpath(xpath)
                        closing_prize = element[0].__getattribute__('text')
                        data[date] = closing_prize
                    except IndexError:
                        data[date] = "No data avialable"  # Some lines may have empty prize values

                    print("read "+str(i)+" lines of data successfully")

                    i += 1

                else:
                    break

            except IndexError:
                break

        driver.close()

        file = open("database/"+TICKER+".txt", "w")
        a = 0
        for ts in sorted(timestamps):
            pvm = ts[6]+ts[7]+"."+ts[4]+ts[5]+"."+ts[0]+ts[1]+ts[2]+ts[3]
            print("PVM: " + pvm + "   PÄÄTÖSKURSSI: " + data[pvm] + "\n", file=file)
            print("Wrote "+str(a+1)+" lines to file. Timestamp: "+str(pvm))
            a += 1

        file.close()


if __name__ == "__main__":
    main()

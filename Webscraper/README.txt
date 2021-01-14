This tool can be used to analyze history data for different stocks on the HEL stock market. 
It was my first experience with webscraping and uses Selenium to gather the market data.

It's tested to work with PyCharm 2020.2.5 and Pyhton 3.8, and requires urllib and selenium 
libraries. 

Webscraper.py is used to create and update the database, and Analyzer.py is the actual tool
to analyze the gathered data.

The analyzer asks for a month to analyze, which must be given using two digits. 
(for example January is entered as 01 and December as 12)

The result is a top10 list with the following format: 


1. NEO1V [Ticker of the stock]    24.48 [Last closing prize]
[History data]2011 : -8.11    2012 : -4.94    2013 : 0.61    2014 : 0.00    2015 : 254.88    2016 : 1.91    2017 : 26.56    2018 : -7.99    2019 : -6.25    2020 : -11.86    

2. SOPRA    13.11
2011 : 0.00    2013 : 16.67    2014 : 45.45    2015 : 8.00    2016 : -5.56    2017 : 33.33    2018 : 3.92    2019 : 12.50    2020 : 3.70    

3. VALOE    12.60
2011 : 6.67    2012 : -10.00    2013 : -11.11    2014 : -33.33    2015 : 0.00    2016 : -8.11    2017 : -8.33    2018 : -8.65    2019 : 184.62    2020 : 14.29    

... 

The history data is displayed in change (%) during the analyzed month for the corresponding year.
All prizes are in EUR (€).
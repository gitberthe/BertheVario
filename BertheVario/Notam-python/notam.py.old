#!/usr/bin/python

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException

options = Options()
options.headless = True
options.add_argument("--window-size=1920,1200")

# ouverture de la page web
driver = webdriver.Firefox()
driver.get("https://sofia-briefing.aviation-civile.gouv.fr/sofia/pages/prepavol.html/")

# clic type de vol lcal
type_de_vol = driver.find_element_by_id('labelVolLocal')
#type_de_vol = driver.find_element(By.XPATH("//span[contains(@class,'v-menubar-menuitem-caption') and contains(text(), 'Local')]"));

#type_de_vol.send_keys('L')
type_de_vol.click()

# clic date de depart
date_de_depart = driver.find_element_by_id('cal_btn')
date_de_depart.click()
date_de_depart.click()

#aujourdhui = date_de_depart.find_element_by_partial_link_text('Aujourd')
#aujourdhui = driver.find_element( By.PARTIAL_LINK_TEXT ,'Aujourd\'hui')
#aujourdhui.click()

# heure de depart
heure_depart = driver.find_element_by_id('id_departureTime_val')
heure_depart.click() # send_keys('2300')
heure_depart.click()

# aeroport de depart
aeroport_de_depart = driver.find_element_by_id('id_aerodeplocal_val')
aeroport_de_depart.send_keys('LFLW')
aeroport_de_depart.click()
aeroport_de_depart.click()

#selection NOTAM
notam = driver.find_element( By.LINK_TEXT, 'Trajet' )
notam.click()


# attente affichage page
delay_sec = 10
try:
    # Wait for the element with the ID of wrapper
    upper_fl = WebDriverWait(driver, delay_sec).until(
      EC.presence_of_element_located((By.ID, 'id_flight_level_upper_val'))
    )
    #print("Element is present in the DOM now")
except TimeoutException:
    print("Element did not show up")


# plafond haut
upper_fl.clear()
upper_fl.send_keys('115')

# rechercher
rechercher = driver.find_element( By.LINK_TEXT, 'Rechercher' )
rechercher.click()
rechercher.click()


# attante affichage page
try:
    # Wait for the element with the ID of wrapper
    print= WebDriverWait(driver, delay_sec).until(
      EC.presence_of_element_located((By.ID, 'id_notamroute_print_button'))
    )
    #print("Element is present in the DOM now")
except TimeoutException:
    print("Element did not show up")

# photo de la page
driver.save_screenshot('screenshot.png')

# ecriture resultat
fichier = open("result.html", "w")
fichier.write( driver.page_source )
fichier.close() ;

driver.quit()

quit()

#
#driver = webdriver.Chrome(options=options,executable_path='/snap/bin/chromium')
#driver.get("https://news.ycombinator.com/login")

#login = driver.find_element_by_xpath("//input").send_keys('USERNAME')
#password = driver.find_element_by_xpath("//input[@type='password']").send_keys('PASSWORD')
#submit = driver.find_element_by_xpath("//input[@value='login']").click()

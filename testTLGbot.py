
import telepot

# Telegram message
def send_tlg_msg(msg):
    ids = ["@buyqawaef2019"]
    bot = telepot.Bot('839455204:AAEURORDcMya-awnUX2NXhe2DFdVAuOOaCc')
    for id in ids:
        try:
           print(bot.sendMessage(str(id), str(msg)))
        except:
           raise

send_tlg_msg("test")

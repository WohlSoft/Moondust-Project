#!/usr/bin/python3

import json

tpl_category = '<p><span class="title1">%s</span></p><br>'
tpl_role = '<p><span class="title2">%s:</span></p>'

img_root_qt = 'qrc://contributors/'
img_root_local = 'contributors/'


class Person:
    def __init__(self):
        self.nickname = None
        self.nicknames = []
        self.description = None
        self.email = None
        self.real_name = None
        self.real_name_alt = []
        self.home_page = None
        self.home_pages = []
        self.avatar = None


class Group:
    def __init__(self):
        self.name = "<unknown>"
        self.people = []


class Category:
    def __init__(self):
        self.name = "<unknown>"
        self.groups = []


def build_people(people, do_local):
    text = ""

    for person in people:
        text += '<p>'

        text += '<span class="who">'
        if person.home_page:
            text += '<a href="%s">' % person.home_page
        if person.nickname:
            text += '"%s"' % person.nickname
        if person.nickname and person.real_name:
            text += ' - %s' % person.real_name
        elif person.real_name:
            text += '%s' % person.real_name
        if person.home_page:
            text += '</a>'

        if len(person.nicknames) > 0:
            text += ' (a.k.a. '
            is_first = True
            for n in person.nicknames:
                if is_first:
                    is_first = False
                else:
                    text += ' / '
                text += n
            text += ')\n'
        text += '</span>'

        if person.email:
            text += ' &lt;<a class="email" href="mailto:%s">%s</a>&gt;' % (person.email, person.email)
        text += '<br>\n'

        if len(person.real_name_alt) > 0:
            text += '('
            is_first = True
            for n in person.real_name_alt:
                if is_first:
                    is_first = False
                else:
                    text += ' / '
                text += n
            text += ')<br>\n'

        if person.description:
            text += '<small>%s</small><br>\n' % person.description

        if person.avatar:
            text += '<img class="leftPos" alt="avatar" src="%s%s">' % \
                    (img_root_local if do_local else img_root_qt, person.avatar)

        text += '</p>\n'
        if person.avatar or person.description:
            text += '<br>\n<br>\n'

    return text


def build_group(group, do_local):
    text = ""

    for g in group:
        text += '<p><span class="title2">%s:</span></p>\n' % g.name
        text += build_people(g.people, do_local)

    return text


def build_page(cats, do_local):
    text = ""

    is_first = True
    for c in cats:
        if is_first:
            is_first = False
        else:
            text += '<br>\n<br>\n'
        text += '<p><span class="title1">%s:</span></p>\n<br>\n\n' % c.name
        text += build_group(c.groups, do_local)

    # Easter egg
    text += '<br>\n<br>\n<br>\n<br>\n<br>\n<br>\n<br>\n'
    text += '<p>\n' \
            '<img class="rightPos" alt="hele" src="%s/FunnyCat.png"><br>\n' \
            '<span style="font-size: 16pt;">喝了</span>\n' \
            '</p>\n' \
            '<br>\n' % \
            (img_root_local if do_local else img_root_qt)
    # Easter egg END

    return text


categories = []

# TODO:
# with open('contributors.json') as json_file:
#     categories = json.load(json_file)

md = Category()
md.name = 'Main developers'

grp = Group()
grp.name = 'Engine, Editor, Configs, Research, Graphics, Documents, Logo'

p = Person()
p.nickname = "Wohlstand"
p.real_name = "Vitaly Novichkov"
p.real_name_alt = ["Виталий Новичков", "ノヴィチコーフ・ヴィターリ"]
p.email = "admin@wohlnet.ru"
p.avatar = "Wohlstand.png"
grp.people.append(p)

md.groups.append(grp)

grp = Group()
grp.name = 'Editor and Engine (Co-Developer); Configs, Research'

p = Person()
p.nickname = "Kevsoft"
p.real_name = "Kevin Waldock"
p.email = "kevin.waldock@gmail.com"
p.avatar = "Kevsoft.png"
grp.people.append(p)

md.groups.append(grp)

categories.append(md)


md = Category()
md.name = 'Retired developers'

grp = Group()
grp.name = 'Early Mac OS X Builds, Debugging, Proofreading'
p = Person()
p.nickname = "CaptainSwag101"
p.real_name = "James Pelster"
p.email = "jpmac26@gmail.com"
grp.people.append(p)
md.groups.append(grp)

grp = Group()
grp.name = 'Graphics, Documents, Proofreading'
p = Person()
p.nickname = "Veudekato"
grp.people.append(p)

md.groups.append(grp)


categories.append(md)


md = Category()
md.name = 'Graphics Designers'

grp = Group()
grp.name = 'Icons for applications and tools, July 2020'
p = Person()
p.nickname = "Tails608"
p.avatar = "Tails608.png"
grp.people.append(p)
md.groups.append(grp)

grp = Group()
grp.name = 'Icons for applications and tools, September 2015'
p = Person()
p.nickname = "Orakel"
grp.people.append(p)
md.groups.append(grp)

grp = Group()
grp.name = 'Icons for applications and tools, November 2014'
p = Person()
p.nickname = "Valtteri"
p.avatar = "Valtteri.png"
grp.people.append(p)
md.groups.append(grp)

grp = Group()
grp.name = 'Icons for applications and tools, August 2014'
p = Person()
p.real_name = "Aristo Lauderdale"
p.email = "gluigix@gmx.de"
p.avatar = "Marina.png"
grp.people.append(p)
md.groups.append(grp)

categories.append(md)


md = Category()
md.name = 'Translations'

grp = Group()
grp.name = 'English'

p = Person()
p.nickname = "CaptainSwag101"
p.real_name = "James Pelster"
grp.people.append(p)

p = Person()
p.nickname = "Wohlstand"
p.real_name = "Vitaly Novichkov"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'French'

p = Person()
p.nickname = "NathanBnm"
p.real_name = "Nathan Bonnemains"
p.home_page = 'https://github.com/NathanBnm'
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Russian'

p = Person()
p.nickname = "Wohlstand"
p.real_name = "Vitaly Novichkov"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'German'

p = Person()
p.nickname = "Kevsoft"
p.real_name = "Kevin Waldock"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Spanish'

p = Person()
p.nickname = "hacheipe399"
p.real_name = "Paredes Fernando"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Italian'

p = Person()
p.nickname = "AndrewPixel"
p.email = "andrewpixelgamer@gmail.com"
p.avatar = "AndrewPixel.png"
grp.people.append(p)

p = Person()
p.nickname = "Darkonius Mavakar"
p.real_name = "Gian Marco Pagano"
p.avatar = "DarkoniusMavakar.png"
grp.people.append(p)

p = Person()
p.nickname = "ImperatoreXx"
p.real_name = "Giuseppe Virdone"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Portuguese (Brazilian)'

p = Person()
p.nickname = "tb1024"
p.real_name = "Carlos Eduardo"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Polish'

p = Person()
p.nickname = "P4VLOWSKEY"
p.real_name = "Karol Pawłowski"
p.email = "karolpaw99@outlook.com"
grp.people.append(p)

p = Person()
p.nickname = "Mario_and_Luigi_55"
p.real_name = "Jakub Kowalski"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Chinese'

p = Person()
p.nickname = "快乐爱的小精灵"
p.email = "lovebodhi@yandex.com"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Dutch'

p = Person()
p.nickname = "Gameinsky"
p.real_name = "Olivier Bloch"
p.email = "blochjunior@gmail.com"
grp.people.append(p)

md.groups.append(grp)


grp = Group()
grp.name = 'Ukrainian'

p = Person()
p.nickname = "h2643"
p.home_page = "https://youtube.com/c/h2643"
p.real_name = "Denis Savonchuk"
p.real_name_alt = ["Денис Савончук"]
p.email = "denis192851@meta.ua"
p.avatar = "h2643.png"
grp.people.append(p)

md.groups.append(grp)

categories.append(md)


md = Category()
md.name = 'Big thanks to'


grp = Group()
grp.name = 'For the general support'

p = Person()
p.nickname = "5438A38A"
grp.people.append(p)

p = Person()
p.nickname = "Horikawa Otane"
p.description = 'Help with early builds before automatic laboratory was been created to ' \
                'produce night builds of PGE applications'
p.avatar = "HorikawaOtane.png"
grp.people.append(p)

p = Person()
p.nickname = "Rednaxela"
p.description = 'Help with LunaTester sub-module'
grp.people.append(p)

p = Person()
p.nickname = "bossedit8"
p.description = 'Help with detail Alpha/Beta-testing, improvement suggestions, ' \
                'and with a videos'
p.avatar = "Bossedit8.png"
grp.people.append(p)

p = Person()
p.nickname = "Enjl"
p.nicknames = ["Emral"]
p.description = 'Help with suggestions to improve the classic events toolbox design ' \
                'to make it more convenient and other features'
p.avatar = "Enjl.png"
grp.people.append(p)

p = Person()
p.nickname = "FanofSMBX"
p.description = "Contributing materials for the research help"
grp.people.append(p)

p = Person()
p.nickname = "Axiom"
p.description = "Testing and for a initial help to produce of Linux builds"
p.nicknames = ["Luigifan2010"]
grp.people.append(p)

p = Person()
p.nickname = "Natsu"
p.description = "Help with old logo"
grp.people.append(p)

p = Person()
p.nickname = "h2643"
p.description = "PGE Forums admin (2014-2016)"
p.avatar = "h2643.png"
grp.people.append(p)

p = Person()
p.nickname = "Squishy Rex"
p.description = "Themes creator and some graphics help"
p.avatar = "SquishyRex.png"
grp.people.append(p)

p = Person()
p.nickname = "Valtteri"
p.description = "Some graphics help"
p.avatar = "Valtteri.png"
grp.people.append(p)

md.groups.append(grp)


categories.append(md)


def clean_string(text):
    out_text = ''
    for line in text.split('\n'):
        line = line.rstrip()
        out_text += line + "\n"
    return out_text


handle = open("template.html", "r")
template = handle.read()
handle.close()

handle = open("credits.css", "r")
style = handle.read()
handle.close()

style = style.replace('\n', '\n        ')
template = template.replace('/*STYLESHEET*/', style)

handle = open("local_test.html", "w")
handle.write(clean_string(template.replace('<!--PEOPLE-->', build_page(categories, True))))
handle.close()

handle = open("../credits.html", "w")
handle.write(clean_string(template.replace('<!--PEOPLE-->', build_page(categories, False))))
handle.close()

print("Completed!")

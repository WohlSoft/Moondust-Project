#!/usr/bin/python3

from typing import NamedTuple
import json

tpl_category = '<p><span class="title1">%s</span></p><br>'
tpl_role = '<p><span class="title2">%s:</span></p>'

img_root_qt = 'qrc://contributors/'
img_root_local = 'contributors/'


class Person(object):
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

    def set_data(self, data):
        for key in data:
            setattr(self, key, data[key])

    def asdict(self):
        out = {}

        if self.nickname:
            out['nickname'] = self.nickname
        if len(self.nicknames) > 0:
            out['nicknames'] = self.nicknames
        if self.description:
            out['description'] = self.description
        if self.email:
            out['email'] = self.email
        if self.real_name:
            out['real_name'] = self.real_name
        if len(self.real_name_alt) > 0:
            out['real_name_alt'] = self.real_name_alt
        if self.home_page:
            out['home_page'] = self.home_page
        if len(self.home_pages) > 0:
            out['home_pages'] = self.home_pages
        if self.avatar:
            out['avatar'] = self.avatar

        return out


class Group(object):
    def __init__(self):
        self.name = '<unknown>'
        self.people = []

    def asdict(self):
        people_dict = []
        for pers in self.people:
            people_dict.append(pers.asdict())
        return {
            'name': self.name,
            'people': people_dict
        }


class Category(object):
    def __init__(self):
        self.name = "<unknown>"
        self.groups = []

    def asdict(self):
        group_dict = []
        for group in self.groups:
            group_dict.append(group.asdict())
        return {
            'name': self.name,
            'groups': group_dict
        }


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

with open('contributors.json') as json_file:
    categories_dict = json.load(json_file)
    for e in categories_dict:
        cat = Category()
        cat.name = e['name']
        print(cat.name)
        for g in e["groups"]:
            grp = Group()
            grp.name = g['name']
            for p in g["people"]:
                pers = Person()
                pers.set_data(p)
                grp.people.append(pers)
            cat.groups.append(grp)
        categories.append(cat)


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

# categories_dicts = []
# for cat in categories:
#     print(cat.name)
#     categories_dicts.append(cat.asdict())
#
# with open("data_file.json", "w") as write_file:
#     json.dump(categories_dicts, write_file, indent=4, ensure_ascii=False)

handle = open("local_test.html", "w")
handle.write(clean_string(template.replace('<!--PEOPLE-->', build_page(categories, True))))
handle.close()

handle = open("../credits.html", "w")
handle.write(clean_string(template.replace('<!--PEOPLE-->', build_page(categories, False))))
handle.close()

print("Completed!")

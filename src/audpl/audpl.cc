/*
 * Audacious playlist format plugin
 * Copyright 2011 John Lindgren
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions, and the following disclaimer in the documentation
 *    provided with the distribution.
 *
 * This software is provided "as is" and without any warranty, express or
 * implied. In no event shall the authors be liable for any damages arising from
 * the use of this software.
 */

#include <stdlib.h>
#include <string.h>

#include <libaudcore/i18n.h>
#include <libaudcore/plugin.h>
#include <libaudcore/audstrings.h>
#include <libaudcore/inifile.h>

static const char * const audpl_exts[] = {"audpl"};

class AudPlaylistLoader : public PlaylistPlugin
{
public:
    static constexpr PluginInfo info = {N_("Audacious Playlists (audpl)"), PACKAGE};

    constexpr AudPlaylistLoader () : PlaylistPlugin (info, audpl_exts, true) {}

    bool load (const char * filename, VFSFile & file, String & title,
     Index<PlaylistAddItem> & items);
    bool save (const char * filename, VFSFile & file, const char * title,
     const Index<PlaylistAddItem> & items);
};

EXPORT AudPlaylistLoader aud_plugin_instance;

class AudPlaylistParser : private IniParser
{
public:
    AudPlaylistParser (String & title, Index<PlaylistAddItem> & items) :
        title (title),
        items (items) {}

    void parse (VFSFile & file)
    {
        IniParser::parse (file);

        /* finish last item */
        if (uri)
            finish_item ();
    }

private:
    String & title;
    Index<PlaylistAddItem> & items;
    String uri;
    Tuple tuple;

    void finish_item ()
        { items.append (std::move (uri), std::move (tuple)); }

    /* no headings */
    void handle_heading (const char * heading) {}

    void handle_entry (const char * key, const char * value)
    {
        if (! strcmp (key, "uri"))
        {
            /* finish previous item */
            if (uri)
                finish_item ();

            /* start new item */
            uri = String (value);
        }
        else if (uri)
        {
            /* item field */
            if (! tuple)
                tuple.set_filename (uri);

            if (strcmp (key, "empty"))
            {
                auto field = Tuple::field_by_name (key);
                if (field == Tuple::Invalid)
                    return;

                auto type = Tuple::field_get_type (field);
                if (type == Tuple::String)
                    tuple.set_str (field, str_decode_percent (value));
                else if (type == Tuple::Int)
                    tuple.set_int (field, atoi (value));
            }
        }
        else
        {
            /* top-level field */
            if (! strcmp (key, "title") && ! title)
                title = String (str_decode_percent (value));
        }
    }
};

bool AudPlaylistLoader::load (const char * path, VFSFile & file, String & title,
 Index<PlaylistAddItem> & items)
{
    AudPlaylistParser (title, items).parse (file);
    return true;
}

bool AudPlaylistLoader::save (const char * path, VFSFile & file,
 const char * title, const Index<PlaylistAddItem> & items)
{
    if (! inifile_write_entry (file, "title", str_encode_percent (title)))
        return false;

    for (auto & item : items)
    {
        if (! inifile_write_entry (file, "uri", item.filename))
            return false;

        const Tuple & tuple = item.tuple;

        if (tuple)
        {
            int keys = 0;

            for (auto f : Tuple::all_fields ())
            {
                if (f == Tuple::Path || f == Tuple::Basename ||
                 f == Tuple::Suffix || f == Tuple::FormattedTitle)
                    continue;

                const char * key = Tuple::field_get_name (f);
                Tuple::ValueType type = tuple.get_value_type (f);

                if (type == Tuple::String)
                {
                    String str = tuple.get_str (f);
                    if (! inifile_write_entry (file, key, str_encode_percent (str)))
                        return false;

                    keys ++;
                }
                else if (type == Tuple::Int)
                {
                    int val = tuple.get_int (f);
                    if (! inifile_write_entry (file, key, int_to_str (val)))
                        return false;

                    keys ++;
                }
            }

            /* distinguish between an empty tuple and no tuple at all */
            if (! keys && ! inifile_write_entry (file, "empty", "1"))
                return false;
        }
    }

    return true;
}

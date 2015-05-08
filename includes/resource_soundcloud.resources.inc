<?php

/**
 * Parse the twitter url.
 *
 * @param $url
 * @param null $widget_id
 * @return bool|string
 */
function resources_resource_soundcloud_parse($url) {

  $track_info = soundcloud_api_get_track_info($url);

  return 'soundcloud://u/' . $track_info->user_id . '/id/' . $track_info->id;
}

/**
 * Check if a YouTube video id is valid.
 *
 * Check against the oembed stream instead of the gdata api site to
 * avoid "yt:quota too_many_recent_calls" errors.
 *
 * @return
 *   Boolean.
 */
function resources_resource_soundcloud_valid_id($id) {
  return TRUE;
}


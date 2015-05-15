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
 * Check if a Soundcloud id is valid.
 *
 * @return
 *   Boolean.
 */
function resources_resource_soundcloud_valid_id($id) {
  return TRUE;
}


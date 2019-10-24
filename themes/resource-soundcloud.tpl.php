<?php

/**
 * @file resources/themes/resource-soundcloud.tpl.php
 *
 * Template file for theme('media_soundcloud').
 *
 * Variables available:
 *  $uri - The media uri for the SoundCloud audio (e.g., soundcloud://v/xsy7x8c9).
 *  $track_id - The unique identifier of the SoundCloud audio (e.g., xsy7x8c9).
 *  $id - The file entity ID (fid).
 *  $url - The full url including query options for the SoundCloud iframe.
 *  $options - An array containing the SoundCloud formatter options.
 *  $width - The width value set in Media: SoundCloud file display options.
 *  $height - The height value set in Media: SoundCloud file display options.
 *  $title - SoundCloud file's title.
 *
 */

?>
<div class="<?php print $classes; ?> media-soundcloud-<?php print $id; ?>">
  <iframe title="Embedded SoundCloud Media" width="<?php print $options['width']; ?>" height="<?php print $options['height']; ?>" scrolling="no" frameborder="no" src="https://w.soundcloud.com/player/?url=https%3A//api.soundcloud.com/tracks/<?php print $track_id; ?>&amp;<?php print $parameters ?>"></iframe>
</div>
</div>

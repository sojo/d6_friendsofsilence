<?php
// $Id: template.php 6910 2010-03-23 19:16:14Z jeremy $

// Fusion - Tranquil

// Override theme_button for expanding graphic buttons
function fusion_tranquil_button($element) {
  // Make sure not to overwrite classes.
  if (isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'] .' '. $element['#attributes']['class'];
  }
  else {
    $element['#attributes']['class'] = 'form-'. $element['#button_type'];
  }
  
  // Wrap visible inputs with span tags for button graphics
  if (stristr($element['#attributes']['style'], 'display: none;') || stristr($element['#attributes']['class'], 'fivestar-submit') || (is_array($element["#upload_validators"]))) {
    return '<input type="submit" '. (empty($element['#name']) ? '' : 'name="'. $element['#name'] .'" ')  .'id="'. $element['#id'].'" value="'. check_plain($element['#value']) .'" '. drupal_attributes($element['#attributes']) ." />\n";
  }
  else {
    return '<span class="button-wrapper"><span class="button"><span><input type="submit" '. (empty($element['#name']) ? '' : 'name="'. $element['#name'] .'" ')  .'id="'. $element['#id'].'" value="'. check_plain($element['#value']) .'" '. drupal_attributes($element['#attributes']) ." /></span></span></span>\n";
  }
}

function phptemplate_taxonomy_links($node, $vid) {
  if (count($node->taxonomy)){
    $tags = array();
    foreach ($node->taxonomy as $term) {
       if ($term->vid == $vid){
          $tags[] = array('title' => $term->name, 'href' => taxonomy_term_path($term), 'attributes' => array('rel' => 'tag'));
       }
}
    if ($tags){
      return theme_links($tags, array('class'=>'links inline'));
    }
  }
}

/**
 * Generate the HTML output for imagefield + imagecache images so they can be
 * opened in a lightbox by clicking on the image on the node page or in a view.
 *
 * This actually also handles filefields + imagecache images too.
 *
 * @param $view_preset
 *   The imagecache preset to be displayed on the node or in the view.
 * @param $field_name
 *   The field name the action is being performed on.
 * @param $item
 *   An array, keyed by column, of the data stored for this item in this field.
 * @param $node
 *   The node object.
 * @param $rel
 *   The type of lightbox to open: lightbox, lightshow or lightframe.
 * @param $args
 *   Args may override internal processes: caption, rel_grouping.
 * @return
 *   The themed imagefield + imagecache image and link.
 
 * Customized to add image body to captions
 
 */
function phptemplate_imagefield_image_imagecache_lightbox2($view_preset, $field_name, $item, $node, $rel = 'lightbox', $args = array()) {
  if (!isset($args['lightbox_preset'])) {
    $args['lightbox_preset'] = 'original';
  }
  // Can't show current node page in a lightframe on the node page.
  // Switch instead to show it in a lightbox.
  $on_image_node = (arg(0) == 'node') && (arg(1) == $node->nid);
  if ($rel == 'lightframe' && $on_image_node) {
    $rel = 'lightbox';
  }
  $orig_rel = $rel;

  // Unserialize into original - if sourced by views.
  $item_data = $item['data'];
  if (is_string($item['data'])) {
    $item_data = unserialize($item['data']);
  }

  // Set up the title.
  $image_title = $item_data['description'];
  $image_title = (!empty($image_title) ? $image_title : $item_data['title']);
  $image_title = (!empty($image_title) ? $image_title : $item_data['alt']);
  if (empty($image_title) || variable_get('lightbox2_imagefield_use_node_title', FALSE)) {
    $node = node_load($node->nid);
    $image_title =  $node->title;
    
    
    /* CUSTOM define image body */
    $image_body = '<p>' . strip_tags($node->body) . '</p>';
  }

  $image_tag_title = '';
  if (!empty($item_data['title'])) {
    $image_tag_title = $item_data['title'];
  }


  // Enforce image alt.
  $image_tag_alt = '';
  if (!empty($item_data['alt'])) {
    $image_tag_alt = $item_data['alt'];
  }
  elseif (!empty($image_title)) {
    $image_tag_alt = $image_title;
  }

  // Set up the caption.
  $node_links = array();
  if (!empty($item['nid'])) {
    $attributes = array();
    $attributes['id'] = 'lightbox2-node-link-text';
    $target = variable_get('lightbox2_node_link_target', FALSE);
    if (!empty($target)) {
      $attributes['target'] = $target;
    }
    $node_link_text = variable_get('lightbox2_node_link_text', 'View Details');
    if (!$on_image_node && !empty($node_link_text)) {
      $node_links[] = l($node_link_text, 'node/'. $item['nid'], array('attributes' => $attributes));
    }
    $download_link_text = check_plain(variable_get('lightbox2_download_link_text', 'Download Original'));
    if (!empty($download_link_text) && user_access('download original image')) {
      $node_links[] = l($download_link_text, file_create_url($item['filepath']), array('attributes' => array('target' => '_blank', 'id' => 'lightbox2-download-link-text')));
    }
  }
	
   /* CUSTOM add image body to caption */
  $caption = $image_title . $image_body;
  if (count($node_links)) {
    $caption .= '<br /><br />'. implode(" - ", $node_links);
  }
  if (isset($args['caption'])) {
    $caption = $args['caption']; // Override caption.
  }

  if ($orig_rel == 'lightframe') {
    $frame_width = variable_get('lightbox2_default_frame_width', 600);
    $frame_height = variable_get('lightbox2_default_frame_height', 400);
    $frame_size = 'width:'. $frame_width .'px; height:'. $frame_height .'px;';
    $rel = preg_replace('/\]$/', "|$frame_size]", $rel);
  }

  // Set up the rel attribute.
  $full_rel = '';
  $imagefield_grouping = variable_get('lightbox2_imagefield_group_node_id', 1);
  if (isset($args['rel_grouping'])) {
    $full_rel = $rel .'['. $args['rel_grouping'] .']['. $caption .']';
  }
  elseif ($imagefield_grouping == 1) {
    $full_rel = $rel .'['. $field_name .']['. $caption .']';
  }
  elseif ($imagefield_grouping == 2 && !empty($item['nid'])) {
    $full_rel = $rel .'['. $item['nid'] .']['. $caption .']';
  }
  elseif ($imagefield_grouping == 3 && !empty($item['nid'])) {
    $full_rel = $rel .'['. $field_name .'_'. $item['nid'] .']['. $caption .']';
  }
  elseif ($imagefield_grouping == 4 ) {
    $full_rel = $rel .'[allnodes]['. $caption .']';
  }
  else {
    $full_rel = $rel .'[]['. $caption .']';
  }
  $class = '';
  if ($view_preset != 'original') {
    $class = 'imagecache imagecache-' . $field_name . ' imagecache-' . $view_preset . ' imagecache-' . $field_name . '-' . $view_preset;
  }
  $link_attributes = array(
    'rel' => $full_rel,
    'class' => 'imagefield imagefield-lightbox2 imagefield-lightbox2-' . $view_preset . ' imagefield-' . $field_name . ' ' . $class,
  );

  $attributes = array();
  if (isset($args['compact']) && $item['#delta']) {
    $image = '';
  }
  elseif ($view_preset == 'original') {
    $image = theme('lightbox2_image', $item['filepath'], $image_tag_alt, $image_tag_title, $attributes);
  }
  elseif ($view_preset == 'link') {
    // Not actually an image, just a text link.
    $image = variable_get('lightbox2_view_image_text', 'View image');
  }
  else {
    $image = theme('imagecache', $view_preset, $item['filepath'], $image_tag_alt, $image_tag_title, $attributes);
  }

  if ($args['lightbox_preset'] == 'node') {
    $output = l($image, 'node/'. $node->nid .'/lightbox2', array('attributes' => $link_attributes, 'html' => TRUE));
  }
  elseif ($args['lightbox_preset'] == 'original') {
    $output = l($image, file_create_url($item['filepath']), array('attributes' => $link_attributes, 'html' => TRUE));
  }
  else {
    $output = l($image, imagecache_create_url($args['lightbox_preset'], $item['filepath']), array('attributes' => $link_attributes, 'html' => TRUE));
  }

  return $output;
}


/**
* Default theme function for all RSS rows.  Gives us a node variable to use in views templates.
*/
function phptemplate_preprocess_views_view_row_rss(&$vars) {
$view = &$vars['view'];
$options = &$vars['options'];
$item = &$vars['row'];

// Use the [id] of the returned results to determine the nid in [results]
$result = &$vars['view']->result;
$id = &$vars['id'];
$node = node_load( $result[$id-1]->nid );

$vars['title'] = check_plain($item->title);
$vars['link'] = check_url($item->link);
$vars['description'] = check_plain($item->description);
//$vars['description'] = check_plain($node->teaser);
$vars['node'] = $node;
$vars['item_elements'] = empty($item->elements) ? '' : format_xml_elements($item->elements);
}


function fusion_tranquil_username($object) {
  if ( $object->uid && function_exists('content_profile_load') ) {
    $profile = content_profile_load('profile', $object->uid);
  }

  if ( !empty($profile) ) {
    $name = $profile->title;
    if ($name > '') {
    	if ($profile->field_profile_private[0]['value'] != 1) {
      	//return l($name, 'users/'. $profile->uid . '/profile', array('title' => t('View user profile.')));
      	return l($name, 'users/'. $profile->uid . '/profile', array('attributes' => array('title' => t('View user profile.'), 'target' => '_blank')));
				//return l($name, 'user/'. $object->uid, array('attributes' => array('title' => t('View user profile.'))));
    	
    	}
    	else {
      	return check_plain($name);
    	}
    }
  }
  
  if ($object->uid && $object->name) {
    // Shorten the name when it is too long or it will break many tables.
    if (drupal_strlen($object->name) > 20) {
      $name = drupal_substr($object->name, 0, 15) .'...';
    }
    else {
      $name = $object->name;
    }

    if (user_access('access user profiles')) {
      $output = l($name, 'user/'. $object->uid, array('attributes' => array('title' => t('View user profile.'))));
    }
    else {
      $output = check_plain($name);
    }
  }
  else if ($object->name) {
    // Sometimes modules display content composed by people who are
    // not registered members of the site (e.g. mailing list or news
    // aggregator modules). This clause enables modules to display
    // the true author of the content.
    if (!empty($object->homepage)) {
      $output = l($object->name, $object->homepage, array('attributes' => array('rel' => 'nofollow')));
    }
    else {
      $output = check_plain($object->name);
    }

    $output .= ' ('. t('not verified') .')';
  }
  else {
    $output = check_plain(variable_get('anonymous', t('Anonymous')));
  }

  return $output;



}

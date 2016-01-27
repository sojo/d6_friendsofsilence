<div class="<?php print $classes; ?>">
<?php if (!empty($invalid_asin)) { print "<div class='invalid_asin'>This item is no longer valid on Amazon.</div>"; } ?>
<?php //if (!empty($mediumimage)) { print l($mediumimage, $detailpageurl, array('html' => TRUE, 'attributes' => array('rel' => 'nofollow', 'target' => '_blank'))); } ?>
<div>
<!---<em>Purchase from Amazon and support Friends of Silence:</em><br />--->
<strong><?php print l($title, $detailpageurl, array('html' => TRUE, 'attributes' => array('rel' => 'nofollow', 'target' => '_blank', 'title' => 'Click to purchase and support Friends of Silence'))); ?></strong><br />
<?php if (!empty($author)) { print '<em>By ' . $author . '</em>'; } ?>
</div>
</div>

<?php
// $Id: views-view-row-rss.tpl.php 3296 2009-05-27 23:08:21Z tim $
/**
* @file views-view-row-rss.tpl.php
* Default view template to display a item in an RSS feed.
*
* @ingroup views_templates


* REFORMATS front page xml feed using a function in the template.php to  make $node available
*/
?>

<item>
<title><?php print $title ?></title>
<link><?php print $link ?></link>
<description><![CDATA[<?php
$subinfo = strip_tags(phptemplate_taxonomy_links($node, 1)) . ' | ' . strip_tags(phptemplate_taxonomy_links($node, 45));
$desc = strip_tags($node->body);
print $subinfo . '<br />' . nl2br(check_plain(trim($desc))) . '<br /><br />' . strip_tags($node->field_quotation_bi_line[0]['value']);
?>]]></description>
<?php $item_elements = ereg_replace('<pubDate>.*</pubDate>', '', $item_elements);  // removes pubdate
$item_elements = ereg_replace('<dc:creator>.*</dc:creator>', '', $item_elements);  // removes author
$item_elements = ereg_replace('<category .*</category>', '', $item_elements);  // removes terms
$item_elements = preg_replace("!^\s+(\D)!m", "\\1", $item_elements);  // removes some blank lines

print $item_elements; 
 ?>
</item>
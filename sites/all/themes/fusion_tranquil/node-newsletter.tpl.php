<?php
// $Id: node.tpl.php 6795 2010-03-16 23:30:49Z jeremy $
?>

<!-- start node.tpl.php -->
<div id="node-<?php print $node->nid; ?>" class="node <?php print $node_classes; ?>">

  <?php if ($page == 1): ?>
  <div class="node-corners rounded-corners"><div class="corner-top-right corner"></div><div class="corner-top-left corner"></div></div><!-- /node-corners -->
  <?php endif; ?>

    <div class="node-inner clearfix">

      <?php if ($title && ($page != 0)): ?>
      <h1 class="title"><?php print $title; ?></h1>
      <?php endif; ?>

	    <?php if ($node_top && !$teaser): ?>
	    <div id="node-top" class="node-top row nested">
	      <div id="node-top-inner" class="node-top-inner inner">
	        <?php print $node_top; ?>
	      </div><!-- /node-top-inner -->
	    </div><!-- /node-top -->
	    <?php endif; ?>
    
      <?php print $picture ?>

      <?php if ($page == 0): ?>
      <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
      <?php endif; ?>
     

      <?php if ($submitted): ?>
      <div class="meta">
        <!-- <span class="submitted"><php print $submitted ?> | <php print phptemplate_taxonomy_links($node, 45); ?></span> -->
        <span class="submitted"><?php print $submitted ?></span>
      </div>
      <?php endif; ?>

      <div class="content clearfix">
        <?php print $node->content['body']['#value'] ?>
      </div>
      
      <?php if($node->content['files']['#value']) { ?>
      	<div class="attachments"><?php print $node->content['files']['#value']; ?></div>
      <?php } ?>

      <?php if ($terms): ?>
      <div class="terms">
      	<?php
      	print '<ul class="links">';
      	foreach ($node->taxonomy as $myterm) {
      		if($myterm->vid !== '45') {
      			$mypath = 'taxonomy/term/' . $myterm->tid;
      			print '<li>' . l($myterm->name, $mypath) . '</li>';
      			};		
      		} 
      	print '</ul>'
      	?>
      </div>
      <?php endif;?>
      
      
      
      <?php if ($links): ?>
      <div class="links">
        <?php print $links; ?>
      </div>
      <?php endif; ?>
      
      <div class="clear">
      	<?php print $node->content['newsletters_node_content_1']['#value'] ?>
      </div>

      <?php if ($node_bottom && !$teaser): ?>
      <div id="node-bottom">
        <?php print $node_bottom; ?>
      </div>
      <?php endif; ?>

    </div><!-- /node-inner -->

  <?php if ($page == 1): ?>
  <div class="node-corners rounded-corners"><div class="corner-bottom-right corner"></div><div class="corner-bottom-left corner"></div></div><!-- /node-corners -->
  <?php endif; ?>

</div>
<!-- /#node-<?php print $node->nid; ?> -->
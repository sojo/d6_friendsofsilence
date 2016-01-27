<?php
?>

<?php if ($build_type == 'content'):?>

  <?php print $content ?>
<?php else:?>
  <?php if ($sender):?>
    <?php print theme('table', array(t('From'), t('To')), array(array('data' => array($sender, $recipients), 'valign' => 'top')), array('width' => '100%'))?>
  <?php else: ?>
    <?php print $recipients ?>
  <?php endif?>

  <br class="clear" />
  <?php if ($extra):?>
    <?php print $extra ?>'<br class="clear" />
  <?php endif?>

  <div id="send-message">
    <?php print $content ?>
  </div>
<?php endif?>

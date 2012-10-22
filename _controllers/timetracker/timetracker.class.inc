<?php

/*
 * @author Carl McDade
 * @copyright Copyright (c) 2010, Carl McDade
 * @since 2010-02-19
 * @version 0.2
 * @license http://opensource.org/licenses/gpl-license.php GNU Public License
 * @link http://www.drupal.se Drupal Sweden
 *
 * This is a beta application and many functions are created to circumvent Drupal's
 * default way of doing things (forms api) because they to not allow for the functionality needed 
 *
 */


class timetracker{
	
	protected $common;

    function __construct()
    {
        /* Test the Common settings */
        $this->common = Common::get_instance();
    }

    function hook_links()
    {
        
        $section = get_class($this);
    	$var[ $section .'_1'] = array(
        		'text' => 'My time', 
        		'path' => 'timetracker/timetracker/user'
        		);
        
        $var[$section .'_2'] = array(
        		'text' => 'My Reports', 
        		'path' => 'timetracker/timetracker/reports'
        		);
        
        $var[$section .'_3'] = array(
        		'text' => 'My Projects', 
        		'path' => 'timetracker/timetracker/myprojects'
        		);

        return $var;
    }
    
    
    
    function user()
    {        
    	global $user;
    	
    	/*
    		js files
    		drupal_add_library('system', 'ui.slider');
    		drupal_add_library('system','ui.datepicker');
    		the jquery add-ons needed for the UI.
    	*/
    	
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/jquery.form.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/jquery.validate.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/additional-methods.js');
        drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/anytime.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/mvc.js');
    	
    	/* menu on the timetracker page from hook_links() function */
		$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
			  	
    	$check_entries = db_fetch_object(db_query("SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d LIMIT 1", array($user->uid)));
			   	
        
			/* limit to five days */ 
		$output = drupal_get_form('timetracker_mytime_');
		
		
		if($check_entries->user_id <= 0)
		{
		 
			
			$output .= '<div id="most-recent-events"><h2>' . t('Most recent entries') . '</h2>' ;
			$table_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => 'Date'),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
			$entry_data[] = array(
				'data' => array(
					array('data' => '', 'class' => 'cell-1-'.$re_record->id), 
					array('data' => '', 'class' => 'cell-2-'.$re_record->id), 
					array('data' => '', 'class' => 'cell-3-'.$re_record->id), 
					array('data' => '', 'class' => 'cell-4-'.$re_record->id),
					array('data' => '', 'class' => 'cell-5-'.$re_record->id),
					array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $re_record->id . '/' . $re_record->event_date, array('attributes' => array('class' => 'delete-time', 'id' => 'del-' . $re_record->id))), 'class' => 'cell-6-'.$re_record->id),
					array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$re_record->id, array('attributes' => array('class' => 'update-recent-time', 'id' => 'upd-' . $re_record->id))), 'class' => 'cell-7-'.$re_record->id) 
				),
				'id' => 'recent-'.$re_record->id,
						  );
			
			$output .= theme('table', $table_header,$entry_data) . '</div>';
			
			
		}
		else
		{
			/* this is linked to the menu system so no title is necessary */
			$variables['title'] = t( "Things you've done so far");

			
			/* list the user projects table */
			$output .=  '<div id="output"></div>';
			
			$output .= self::dailyreport();
									
			
		}
		
    	$variables['content'] = $output;
    	return _view('default', $variables);	
        
    }
    
    function dailyreport($today = FALSE, $date = NULL)
    {
    	global $user;
    	
    	$total_hours = 0;
    	/* create select list of projects */
		// get vocabulary as options list
		$voc = variable_get('timetracker_projects', 'projects');
		$tree = taxonomy_get_tree($voc,0,NULL,true);
			   	
    	
    	if($today == TRUE)
    	{
    		$table_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => 'Date'),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
    		
    		/* limit to five days */
    		$all = db_query("SELECT *
			FROM {timetracker_project_events} e WHERE e.user_id = %d AND event_date = '" . $date. "' ORDER BY created DESC LIMIT 1", array($user->uid));
			
			while ($record = db_fetch_object($all))
			{
					  
				  $output = '<tr class="event-added" id="recent-' . $record->id . '">' .
					  		'<td class="cell-1-' . $record->id . '">' . $record->id . '</td>' . 
							'<td class="cell-2-' . $record->id . '">' . $record->project_user_task . '</td>' . 
							'<td class="cell-3-' . $record->id . '">' . $record->project_name . '</td>' . 
							'<td class="event-date cell-4-' . $record->id . '"><a href="#'. $record->event_date .'">' . $record->event_date . '</a></td>' .
							'<td class="cell-5-' . $record->id . '">' . $record->project_hours . '</td>' .
							'<td class="cell-6-' . $record->id . '">' .
									l(t('delete'), 'timetracker/timetracker/delete_time/' . $record->id . '/' . $record->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $record->id))) . '</td>' .
							'<td class="cell-7-' . $record->id . '">' .
									l(t('update'), '', array('attributes' => array('class' => array('update-recent-time'), 'id' => 'upd-' . $record->id))) . '</td>' .					  
							'</tr>'. "\n" .
							'<tr id="recent-form-row-'.$record->id .'" style="display:none" class=" temp-form">' . 
							'<td colspan="7"><div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
								'<form method="post" id="re-update-entry-'.$record->id.'" action="../timetracker/timetracker/update_mytime">'.
								'<table><tr>' .
								'<td><label>'.t('Task').'<input type="textfield" id="re-task-'.$record->id.'" name="task" value="'.$record->project_user_task.'" size="24"></label></td>' .
								'<td><label>' . t('Projects') . '' . '</label></td>' .
								'<td>' .
								'<input type="hidden" name="form-id" id="re-form-id-'.$record->id.'" value="'.$record->id.'"></td>' .
								'<td><label>'.t('Hours').'<input type="textfield" id="re-hours-'.$record->id.'" name="hours" size="2" value="'.$record->project_hours.'"></label>' .
								'<td><input type="button" class="recentUpdate" value="update"></td>' .
								'<td class="indicator"><div class="working-indicator"></div></td>' .
								'</tr></table>'.
								'</form>' .
							 '</td></tr>';	  
			}
						 
		  	return $output;
			
    	}
    	else
    	{
    	
    		
    	$recent_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => 'Entry Date'),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
    		
		
		
		
		/* show the user 5 of the most recent events added */
    	$recent = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d ORDER BY created DESC LIMIT 5', array($user->uid));
		
		
		while ($re_record = db_fetch_object($recent))
		{
				  
			$selected = '';  
			$terms = '<select id="re-projects-'.$re_record->id.'" name="projects">'; 
			$terms .= '<option value="0"></option>';
					foreach ($tree as $key => $term)
					{
						if($term->tid === $re_record->project_id)
						{
							$selected = 'selected="selected"';
						}
						$terms .= '<option value="'.$term->tid.'" '.$selected.'>' . $term->name . '</option>';
						$selected = '';
					}
			$terms .= '</select>'; 	  
			
			$event_data[] = array(
				'data' => array(
					array('data' => $re_record->id, 'class' => 'cell-1-'.$re_record->id), 
					array('data' => $re_record->project_user_task, 'class' => 'cell-2-'.$re_record->id), 
					array('data' => $re_record->project_name, 'class' => 'cell-3-'.$re_record->id), 
					array('data' => l($re_record->event_date, '',  array('fragment' => $re_record->event_date, 'external' => TRUE)), 'class' => 'cell-4-'.$re_record->id),
					array('data' => $re_record->project_hours, 'class' => 'cell-5-'.$re_record->id),
					array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $re_record->id . '/' . $re_record->event_date, array('attributes' => array('class' => 'delete-time', 'id' => 'del-' . $re_record->id))), 'class' => 'cell-6-'.$re_record->id),
					array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$re_record->id, array('attributes' => array('class' => 'update-recent-time', 'id' => 'upd-' . $re_record->id))), 'class' => 'cell-7-'.$re_record->id) 
				),
				'id' => 'recent-'.$re_record->id,
						  );
			$event_data[] = array(
							'data' => array(
								
								array('data' => '<div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
									'<form method="post" id="re-update-entry-'.$re_record->id.'" action="../timetracker/timetracker/update_mytime">'.
									'<table><tr>' .
									'<td><label>'.t('Task').'<input type="textfield" id="re-task-'.$re_record->id.'" name="task" value="'.$re_record->project_user_task.'" size="12"></label></td>' .
									'<td><label>' . t('Projects') . '' . $terms . '</label></td>' .
									'<td>' .
									'<input type="hidden" name="form-id" id="re-form-id-'.$re_record->id.'" value="'.$re_record->id.'"></td>' .
									'<td><label>'.t('Hours').'<input type="textfield" id="re-hours-'.$re_record->id.'" name="hours" size="2" value="'.$re_record->project_hours.'"></label>' .
									'<td><input type="button" class="recentUpdate" value="update"></td>' .
									'<td class="indicator"><div class="working-indicator"></div></td>' .
									'</tr></table>'.
									'</form>', 
									'colspan' => 7)),
								'id' => 'recent-form-row-'.$re_record->id,
								'style' => 'display:none',
						);
				  
				  
		}	
		
		$output[] = '<div id="most-recent-events"><h2>' . t('Most recent entries') . '</h2>' . theme('table', $recent_header,$event_data) . '</div>';
		
		
		$table_header = array(
				array('data' => 'ID'),
				array('data' => 'Task'),
				array('data' => 'Project'),
				array('data' => ''),
				array('data' => 'Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
		
		/*  limit to five days */
    	$all = db_query("SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d AND event_date <= '" . date("Y-m-d H:i:s") ."' ORDER BY event_date DESC, id DESC", array($user->uid));
				
		while ($record = db_fetch_array($all))
		{
			$groups[$record['event_date']][] = $record;
		}
			
		
		foreach($groups as $key => $group)
		{
			//sections by date
			$output[] = '<div id="table-' . $key . '">' . 
		
			'<div class="table-label"><a name="' . $key . '" href=""><span class="date">' . $key . '</span></a><span class="hours"></span></div>';
			
			$date = $key;
	
		
		foreach ($group as $record)
		{
				$selected = '';  
				$terms = '<select id="projects-'.$record['id'].'" name="projects">'; 
				$terms .= '<option value="0"></option>';
					foreach ($tree as $key => $term)
					{
						if($term->tid === $record->project_id)
						{
							$selected = 'selected="selected"';
						}
						$terms .= '<option value="'.$term->tid.'" '.$selected.'>' . $term->name . '</option>';
						$selected = '';
					}
				$terms .= '</select>';  
			
				$row_data[] = array(
						  'data' => array(
						  	  array('data' => $record['id'], 'class' => 'cell-1-'.$record['id']), 
						  	  array('data' => $record['project_user_task'], 'class' => 'cell-2-'.$record['id']), 
						  	  array('data' => $record['project_name'], 'class' => 'cell-3-'.$record['id']),
						  	  array('data' => '<input type="hidden" name="my_projects_id" id="my-projects-id" value="' .$record['project_id'] . '">', 'class' => 'cell-4-'.$record['id']),
						  	  array('data' => $record['project_hours'], 'class' => 'cell-5-'.$record['id']),
						  	  array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $record['id'] . '/' . $record['event_date'], array('attributes' => array('class' => 'delete-time', 'id' => 'del-' . $record['id']))), 'class' => 'cell-6-'.$record['id']),
						  	  array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$record['id'], array('attributes' => array('class' => 'update-time', 'id' => 'upd-' . $record['id']))), 'class' => 'cell-7-'.$record['id']) 
				  
						  	  ),
						  'class' => 'row-'.$record['id'],
						  );
				  $row_data[] = array(
							'data' => array(
								
								array('data' => '<div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
									'<form method="post" id="update-entry-'.$record['id'].'" action="http://drupal.se/timetracker/timetracker/update_time">'.
									'<table><tr>' .
									'<td><label>'.t('Task').'<input type="textfield" id="edit-task-'.$record['id'].'" name="task" value="'.$record['project_user_task'].'" size="24"></label></td>' .
									'<td><label>' . t('Projects') . '' . $terms . '</label></td>' .
									'<td>' .
									'<input type="hidden" name="form-id" id="form-id-'.$record['id'].'" value="'.$record['id'].'"></td>' .
									'<td><label>'.t('Hours').'<input type="textfield" id="edit-hours-'.$record['id'].'" name="hours" size="2" value="'.$record['project_hours'].'"></label>' .
									'<td><input type="button" id="sendUpdate" value="update"></td>' .
									'<td class="indicator"><div class="working-indicator"></div></td>' .
									'</tr></table>'.
									'</form>', 
									'colspan' => 7)),
								'id' => 'form-row-'.$record['id'],
								'style' => 'display:none',
						);
				  
				  $total_hours += $record['project_hours'];
				  
		}
		
		$row_data[] = array(
							'data' => array(
								l(t('export'),'timetracker/timetracker/myreport_csv/' . $date),
								'',
								'',
								'Total Hours:', 
								array('data' => $total_hours , 'colspan' => 3, 'id' => 'total-user-hours')),
							'id' => 'total-row-'.$date,
						);	
		
		
		/* Format and print out the table. */
		
		
		$output[] = theme('table', $table_header,$row_data) . '</div>';
		
		unset($row_data);
		$total_hours = 0;
		
		}
		}
    					 
    	return implode('', $output);
    	
    }
    
    /* this form posts to myreport() via ajax */

    function reports()
    {  	
    	global $user;
    	
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/jquery.form.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/anytime.js');
    	drupal_add_js(drupal_get_path('module', APP_MODULE ) . '/js/mvc.js');   	
    	
    	
    	/* links */
        $variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
        
        $check_entries = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d', array($user->uid));
			
		
		if($check_entries->num_rows <= 0)
		{
		
			/* monthly totals for this user by project */
			$variables['content'] = t('No reports are available.');

			return _view('default', $variables);
		}
		
		/* title */
        $variables['title'] = 'My time reports';
		
        
        /* form for choosing a project report */
        $output = drupal_get_form('timetracker_myreports_');
        
    	/* export csv */
		$output .= '<div id="export-form" style="display:none">' . drupal_get_form('timetracker_myreports_csv_') . '</div>';
			
    	/* monthly totals for this user by project */
        $variables['content'] = $output;

        return _view('default', $variables);
        
    }
    
    /* this is the response to the ajax call for the reports() function/form */
    
    function myreport($project = NULL)
    {
    	global $user;
    	
    	$total_hours = 0;
    	
    	$start = $_POST['start_date'];
    	$end = $_POST['end_date'];
    	
    	/* check for post request */
    	if(isset($_POST['my_projects_id']) && $_POST['my_projects_id'] != '')
    	{
    		$project = check_plain($_POST['my_projects_id']);   		
    		
			/*  get the term name */
			$project_info = taxonomy_get_term($project);
			
			if($start && !empty($start))
			{
				$result = db_query("SELECT *
				FROM {timetracker_project_events} e WHERE e.event_date BETWEEN '%s' AND '%s' AND e.user_id = %d AND e.project_id = %d ORDER BY created DESC", array($start, $end, $user->uid, $project));
			}
			else
			{
				//
				$result = db_query("SELECT *
				FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.project_id = %d ORDER BY created DESC", array($user->uid, $project));
			
			}
				
    	}
    	else
    	{
    		//
    		$result = db_query('SELECT *
				FROM {timetracker_project_events} e WHERE e.user_id = %d AND event_date = date(now()) ORDER BY created DESC', array($user->uid));
			
    	}
    		
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		
		
		$table_header = array(
				array('data' => 'ID', '' => 'id', 'sort' => 'asc'),
				array('data' => 'Task', '' => 'project_user_task'),
				array('data' => 'Date', '' => 'event_date'),
				array('data' => 'Users', '' => 'project_users'),
				array('data' => 'Hours', '' => 'project_hours'),
				array('data' => 'Operations', '' => '', 'colspan' => 3),			
				);
		
		while ($record = db_fetch_array($result))
		{
				  
				/* create select list of projects */
				// get vocabulary as options list
				$voc = variable_get('timetracker_projects', 'projects');
				$tree = taxonomy_get_tree($voc,0,NULL,true);
				
				$pj_users = db_query('SELECT DISTINCT user_id FROM {timetracker_project_events}');
					
	
				/* Result is returned as a iterable object that returns a stdClass object on each iteration */
							
				while ($pj_record = db_fetch_object($pj_users))
				{
					$total_project_users += $pj_record->user_id;
				}	
				
				
			
				$selected = '';  
				$terms = '<select id="projects-'.$record['id'].'" name="projects">'; 
				$terms .= '<option value="0"></option>';
					foreach ($tree as $key => $term)
					{
						if($term->tid === $record->project_id)
						{
							$selected = 'selected="selected"';
						}
						$terms .= '<option value="'.$term->tid.'" '.$selected.'>' . $term->name . '</option>';
						$selected = '';
					}
				$terms .= '</select>';  
			
				$row_data[] = array(
						  'data' => array(
						  	  array('data' => $record['id'], 'class' => 'cell-1-'.$record['id']), 
						  	  array('data' => $record['project_user_task'], 'class' => 'cell-2-'.$record['id']), 
						  	  array('data' => $record['event_date'], 'class' => 'cell-3-'.$record['id']),
						  	  array('data' => $total_project_users, 'class' => 'cell-4-'.$record['id']),
						  	  array('data' => $record['project_hours'], 'class' => 'cell-5-'.$record['id']),
						  	  array('data' => '<input type="hidden" name="list-projects_id" id="list-projects-id" value="' .$record['project_id'] . '">', 'class' => 'cell-6-'.$record['id']),
						  	  array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $record['id'] . '/' . $record['event_date'], array('attributes' => array('class' => 'delete-time', 'id' => 'del-' . $record['id']))), 'class' => 'cell-7-'.$record['id']),
						  	  array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$record['id'], array('attributes' => array('class' => 'update-time', 'id' => 'upd-' . $record['id']))), 'class' => 'cell-8-'.$record['id']) 
				  
						  	  ),
						  'class' => 'row-'.$record['id'],
						  );
				  $row_data[] = array(
							'data' => array(
								
								array('data' => '<div style="text-align:right;cursor:pointer;" class="form-close">' . t('close') . ' x</div>' .
									'<form method="post" id="update-entry-'.$record['id'].'" action="http://drupal.se/timetracker/timetracker/update_time">'.
									'<table><tr>' .
									'<td><label>'.t('Task').'<input type="textfield" id="edit-task-'.$record['id'].'" name="task" value="'.$record['project_user_task'].'" size="24"></label></td>' .
									'<td><label>' . t('Projects') . '' . $terms . '</label></td>' .
									'<td>' .
									'<input type="hidden" name="form-id" id="form-id-'.$record['id'].'" value="'.$record['id'].'"></td>' .
									'<td><label>'.t('Hours').'<input type="textfield" id="edit-hours-'.$record['id'].'" name="hours" size="2" value="'.$record['project_hours'].'"></label>' .
									'<td><input type="button" id="sendUpdate" value="update"></td>' .
									'<td class="indicator"><div class="working-indicator"></div></td>' .
									'</tr></table>'.
									'</form>', 
									'colspan' => 7)),
								'id' => 'form-row-'.$record['id'],
								'style' => 'display:none',
						);
				  
				  $total_hours += $record['project_hours'];
		}
		
		$row_data[] = array(
							'data' => array(
								l(t('export'),'timetracker/timetracker/myreport_csv/' . $start .'/' . $end . '/' . $project),
								'',
								'Total Hours:', 
								array('data' => $total_hours , 'colspan' => 4, 'id' => 'total-user-hours')),
							'id' => 'total-row-'.$date,
						);
		
		
		/* Format and print out the table. */
		$output .= theme('table', $table_header,$row_data);
		
    					 
    	print $output;
    	
    }
    
    /* the response to the call for the export form */
    
    function myreport_csv($result,  $filename = NULL)
    { 
    	
    	global $user;
    	 	
    	$total_hours = 0;
    	
    	/* clean input */
    	if(isset($_POST['my_report_id']) && $_POST['my_report_id'] != '')
    	{
    		$project = check_plain($_POST['my_report_id']);
    		/*  get the term name */
    		$project_info = taxonomy_term_load($project);
    		
    		$row_sql = "SELECT *
			FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.project_id = %d ORDER BY created DESC";
			
			$row_conditions = array($user->uid, $project);
						
			/* build the file name */
			$filename = strtolower($user->name . '-project-' . $project_info->tid . '-' . date("Y-m-d") . '.txt');
			
			/* cleanup file name from commonly used special characters */
			$not_used = array(" ", "_", "&","[","]");
			$filename = str_replace($not_used, "-", $filename);
    	}
    	elseif(is_numeric(arg(5)))
    	{
    		//Check for get request
    		$start = arg(3);
    		$end = arg(4);
    		$project = arg(5);   		
    					
			$row_sql = "SELECT *
				FROM {timetracker_project_events} e WHERE e.event_date BETWEEN '%s' AND '%s' AND e.user_id = %d AND e.project_id = %d ORDER BY created DESC";
				
			$row_conditions = array($start, $end, $user->uid, $project);
			
			/* build the file name */
			$filename = strtolower($user->name . '-entry-dates-' . $start . '--' . $end .'.txt');
			
			/* cleanup file name from commonly used special characters */
			$not_used = array(" ", "_", "&","[","]");
			$filename = str_replace($not_used, "-", $filename);
				
    	}
    	else
    	{
    		$event_date = arg(3);   		
    		
    		$row_sql = "SELECT * 
    			FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.event_date = '%s' ORDER BY created DESC";
			
			$row_conditions = array($user->uid, $event_date);
			
    		/* build the file name */
			$filename = strtolower($user->name . '-entry-date-' . $event_date . '.txt');
			
			/* cleanup file name from commonly used special characters */
			$not_used = array(" ", "_", "&","[","]");
			$filename = str_replace($not_used, "-", $filename);		 	
    	
    	}
    	
    	/*  set content headers */
    	drupal_set_header('Content-Type: text/csv');
    	drupal_set_header('Content-Disposition: attachment; filename=' . $filename);
    	    	
    	
    			
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
    	
		/* field headers */
    	$count = 0;
    	
    	$result = db_query($row_sql, $row_conditions);
    	
    	while($head = db_fetch_array( $result ))
    	{
    		foreach($head as $key => $value){
			  $header[$key] = $key; 
			}
			
			if($x > 0)
			{
				/* iterate only one record to get the database field names */
				break;
			}
    	}
    	
    	$output =  implode(',', $header) ."\r\n";
    	
    	/* field values */
    	
    	$result = db_query($row_sql, $row_conditions);
			
    	while($row = db_fetch_array($result))
    	{
    		
			foreach($row as $value){
			  $values[] = '"' . str_replace('"', '""', decode_entities(strip_tags($value))) . '"'; 
			}
			
			$output .= implode(',', $values) . "\r\n";
			
			unset($values);
		}
		
		print $output ;
		
	}
    
	/* this is a list to inform the user about the projects available */
    
    function projects()
    {
          	
    	
    	/* links */
    	$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
    	
    	/* check for available projects */
    	$check_projects = db_fetch_object(db_query("SELECT * FROM {timetracker_projects} AS pj"));
		
		/* show a list of the taxonomy terms used in the the vocbulary */
    	$output = drupal_get_form('timetracker_projects_add_', 'timetracker/timetracker/project_save');
    	
    	
    	if($check_projects->num_rows == 0)
		{
		
			/* monthly totals for this user by project */
			$variables['content'] = $output . ('No projects are available');

			return _view('default', $variables);
		}
		
    	/* title */
        $variables['title'] = 'My projects';
    	
    	$output .= '<div id="project-list">' . t('A list of available projects and their descriptions:') . '</div>';
    	
    	/* get vocabulary as options list */
		$voc = variable_get('timetracker_projects', 0);
		$tree = taxonomy_get_tree($voc,0,NULL,true);
				
		foreach ($tree as $key => $term)
		{
			$output .= '<dl><dt>' . $term->name . '</dt><dd>' . $term->description  . '</dd></dl>';												
		}
    	
    	/* show a list of the taxonomy terms used in the the vocbulary */
        $variables['content'] = $output;
        
        return _view('default', $variables);
    }
    
    
    function project_add()
    {
    	
    	/* links */
    	$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
    	
    	
    	/* show a list of the taxonomy terms used in the the vocbulary */
    	$output = drupal_get_form('timetracker_projects_add_', 'timetracker/timetracker/project_save');
    	
        $variables['content'] = $output;
        
        return _view('default', $variables);
    }
    
    function project_save($tid=NULL)
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = variable_get('timetracker_projects', NULL);	

    	$term = array();
		$term['name'] = check_plain($_POST['project_name']);
		$term['description'] = check_plain($_POST['project_desc']);
		$term['vid'] = $voc;
		
		$group = isset($_POST['project_group']) ? check_plain($_POST['project_group']) : NULL;
		$ext_key = check_plain($_POST['project_external_key']);
		$status = check_plain($_POST['project_status']);
		
		$check_key = db_query("SELECT * FROM {timetracker_projects} WHERE project_external_key='%s'", array($ext_key));
		
		if($check_key->num_rows > 0)
		{
				drupal_set_message( t('project key must be unique. "') . $ext_key . t('" used.') );
				drupal_goto(url('timetracker/timetracker/project_add'));
				return;
		}
		
		/* save to the Drupal taxonomy system which creates a term/project */
		taxonomy_save_term($term);		
		
		if(!empty($term['tid']))
		{
			drupal_set_message( t('project taxonomy created') ); 
		
			/* get the term and information that was inserted into the database */
			$new = $term['tid'];
    			
			$term_info = taxonomy_get_term($new);
			
			$project_info = db_query("INSERT INTO {timetracker_projects} 
				(project_id,project_name,project_desc,project_group_id,project_external_key,created_by,created,changed,status, vid) 
				VALUES(" . 
				"'" . $new . "'," .
				"'" . $term_info->name ."',". 
				"'" . $term_info->description . "'," . 
				"'" . $group . "'," .  
				"'" . $ext_key . "'," . 
				"'" . $user->uid . "'," .
				"'" . date("Y-m-d H:i:s") . "'," .
				"'" . date("Y-m-d H:i:s") . "'," .
				"'" . $status . "'," .
				"'" . $voc . "')");
			
			if($project_info != FALSE)
			{
				drupal_set_message( t('project added to timetracker') );
			}
			
		 }
				
		 
		 drupal_goto(url('timetracker/timetracker/user'));
    }
    
    
    
    /*  the response to the mini-form for updating an entry */
    
    function update_time()
    {
    	
    	global $user;    	
    	
    	/* id title hours date */
    	$pid = check_plain($_POST['pid']);
    	$id = check_plain($_POST['id']);
    	$hours = check_plain($_POST['hours']);
    	$task = check_plain($_POST['task']);
    	
    	/*  get the term name */
    	$project = check_plain($_POST['project']);
		$project_info = taxonomy_get_term($project);
		
		$event_update = db_query("UPDATE {timetracker_project_events} SET 
				project_user_task ='%s'," . 
				"project_name = '%s'," . 
				"project_id = %d," . 
				"project_hours = %d" . ","  .
				"changed = '%d'" . 
				" WHERE id = %d AND user_id = %d", array($task,$project_info->name,$project_info->tid,$hours,date("Y-m-d H:i:s"),$id,$user->uid));
		
		if($event_update > 0)
		{
		 
		  /* limit to five days */
    	$result = db_query("SELECT *
			FROM {timetracker_project_events} e WHERE e.id = :id LIMIT 1", array(':id' => $id));
			
		$all = $result->fetchObject();
			
		
			$output = '<tr class="event-added" id="row-' . $all->id . '">' .
					  '<td class="cell-1-' . $all->id . '">' . $all->id . '</td>' . 
					  '<td class="cell-2-' . $all->id . '">' . $all->project_user_task . '</td>' . 
					  '<td class="cell-3-' . $all->id . '">' . $all->project_name . '</td>' . 
					  '<td class="event-date cell-4-' . $all->id . '">' . $all->event_date . '</td>' .
					  '<td class="cell-5-' . $all->id . '">' . $all->project_hours . '</td>' .
					  '<td class="cell-6-' . $all->id . '">' .
					  	l(t('delete'), 'timetracker/timetracker/delete_time/' . $all->id . '/' . $all->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $all->id))) . '</td>' .
					  '<td class="cell-7-' . $all->id . '">' .
						l(t('update'), APP_MODULE .'/timetracker/update_time/'.$all->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $all->id))) . '</td>' .
					  '</tr>';
			$row_id = $all->id;
			
		/* update the hours total for the day */
		$result_hours = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = :uid AND e.event_date = :eid', array(':uid' => $user->uid,':eid' => $all->event_date));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		$all_hours = $result_hours->fetchAll();
		
		
		foreach ($all_hours as $record)
		{
			$total_hours += $record->project_hours;
		}	
		
    	
    	/* ajax response */
    	print $all->id . '}{' . $output . '}{' . $total_hours . '}{' . $all->event_date;
    	  
    	}
    	else
    	{
    		print t('error row not updated');
    	}
    }
    
    /* the response to the delete link on the list of entries */
    
    function delete_time()
    {
    	global $user;
    	
    	/* remove record from user database */
    	$id = check_plain(arg(3));
    	
    	$deleted = db_query("DELETE FROM {timetracker_project_events} WHERE id=%d", array($id));
		  
		$total_hours = 0;
    	
    	/* clean input */
    	$project = check_plain(arg(4));
    	
    	/*  get the term name */
    	$project_info = taxonomy_get_term($project);
    	
    	$result = db_query('SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.project_id = %d ORDER BY created DESC', array($user->uid,$project));
				
		/* Result is returned as a iterable object that returns a stdClass object on each iteration */
		
		
		while ($record = db_fetch_object($record))
		{
			$total_hours += $record->project_hours;
		}
    	
    	print $total_hours;
    }
    
    /* the response to creating a time entry for a project */
    
    function myprojects()
    {
        global $user;  	
    	
    	/* links */
    	$variables['links'] = _menu_links($this->common->get_hooks('hook_links'), ' | ');
    	
    	/* check for available projects */
    	$projects = db_query("SELECT * FROM {timetracker_projects} WHERE created_by = %d", array($user->uid));
		
		/* show a list of the taxonomy terms used in the the vocbulary */
    	$output = drupal_get_form('timetracker_projects_add_', 'timetracker/timetracker/project_save');
    	
    	
    	if($projects->num_rows <= 0)
		{
		
			/* monthly totals for this user by project */
			$variables['content'] = $output . ('No projects are available');

			return _view('default', $variables);
		}
		
    	/* title */
        $variables['title'] = 'My projects';
    	
    	$output .= '<div id="project-list">' . t('A list of your projects and their descriptions:') . '</div>';
    	
    	/* get vocabulary as options list */
		$voc = variable_get('timetracker_projects', 0);
		
		$tree = taxonomy_get_tree($voc,0,NULL,true);
		
		$total_project_hours = 0;
		$total_project_users = 0;
		
		$table_header = array(
				array('data' => 'ID'),
				array('data' => 'Name'),
				array('data' => 'Total Users'),
				array('data' => 'Total Hours'),
				array('data' => 'Operations','colspan' => 2),			
				);
		
		
		while ($p_record = db_fetch_object($projects))
		{
			$proj_array[$p_record->project_id] = $p_record->project_id;				
		}
		
		/* check for available projects */
    	$projects = db_query("SELECT * FROM {timetracker_projects} WHERE created_by = %d", array($user->uid));
		
				
		while ($record = db_fetch_object($projects))
		{
			$pj_users = db_query('SELECT project_hours, COUNT(DISTINCT user_id) as us FROM {timetracker_project_events} e WHERE e.project_id = %d', array($proj_array[$record->project_id]));
			$pj_hours = db_query('SELECT project_hours FROM {timetracker_project_events} e WHERE e.project_id = %d', array($proj_array[$record->project_id]));
			

			/* Result is returned as a iterable object that returns a stdClass object on each iteration */
						
			while ($pj_record = db_fetch_object($pj_users))
			{
				$total_project_users += $pj_record->us;
			}

			while ($pj_record = db_fetch_object($pj_hours))
			{
				$total_project_hours += $pj_record->project_hours;
			}			
			
			$row_data[] = array(
						  'data' => array(
						  	  array('data' => $record->project_id, 'class' => 'cell-1-'.$record->id), 
						  	  array('data' => $record->project_name, 'class' => 'cell-2-'.$record->id),
						  	  array('data' => $total_project_users, 'class' => 'cell-2-'.$record->id),
						  	  array('data' => $total_project_hours, 'class' => 'cell-3-'.$record->id),
						  	  array('data' => l(t('delete'), 'timetracker/timetracker/delete_mytime/' . $record->id . '/' . $record->event_date, array('attributes' => array('class' => 'delete-time', 'id' => 'del-' . $record->id))), 'class' => 'cell-4-'.$record->id),
						  	  array('data' => l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$record->id, array('attributes' => array('class' => 'update-time', 'id' => 'upd-' . $record->id))), 'class' => 'cell-5-'.$record->id) 
				  
						  	  ),
						  'class' => 'row-'.$record->id,
						  );
			
			$total_project_hours = 0;
			$total_project_users = 0;		
				  
		}
		
		$row_data[] = array(
							'data' => array(
								l(t('export'),'timetracker/timetracker/myreport_csv/'),
								'', 
								array('data' => '' , 'colspan' => 2, 'id' => 'total-user-hours')),
							'id' => 'total-row-'.$date,
						);	
				
		/* Format and print out the table. */		
		
		$output .= '<div>'. theme('table', $table_header,$row_data) . '</div>';   	
		
        $variables['content'] = $output;
        
        return _view('default', $variables);
    }
    
    function mytime()
    {
    	global $user;
    	
    	/*
    		the form is using the term id so getting the term object to put the term
    		info into the database
    	*/
    	$project_info = taxonomy_get_term($_POST['project_id']);
    	
    	/* clean the user input */
    	$user_task = $_POST['user_task'];
    	
    	/* clean user input */
    	$project_hours = $_POST['hours_used'];
    	
    	/* clean user input */
    	$event_date = $_POST['event_date'];
    	   	
    	
    	/* add record to user database */
    	global $user;
    	
    	/* check for first entry */
    	$first_entry = db_query("SELECT COUNT(user_id) FROM {timetracker_project_events} WHERE user_id = %d",array($user->uid));
    	
    	$event = db_query("INSERT INTO {timetracker_project_events} 
				(user_id,project_id,project_name,project_user_task,event_date,project_hours,created,changed) 
				VALUES(" . 
				"'" . $user->uid . "'," .
				"'" . $project_info->tid ."',". 
				"'" . $project_info->name . "'," . 
				"'" . $user_task . "'," .  
				"'" . $event_date . "'," . 
				"'" . $project_hours . "'," . 
				"'" . date("Y-m-d H:i:s") . "'," .
				"'" . date("Y-m-d H:i:s") . "')");
    	   	
				
		/* inform the user */
		//drupal_set_message(t( 'Confirming ') . $project_hours . t(' task hours added to ') . $project_info->name . t(' for ') . $event_date );
		
		/* show the updated report */
		
		//print $first_entry->user_id;
		if($first_entry->num_rows <= 0 )
		{
			$mode = FALSE;
		}
		else
		{
			$mode = TRUE;
		}
		
		
    	$all = db_query("SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.event_date = '%s'", array($user->uid,$event_date));
				
		
		while($record = db_fetch_object($all))
		{
			$total_hours += $record->project_hours;
		}
		
		print $event_date.'}{'. self::dailyreport($mode, $event_date) .'}{' . $total_hours ;
    	  	
    }
    
    /* the response to the delete link on the list of entries */
    
    function delete_mytime()
    {
    	global $user;
    	
    	/* remove record from user database */
    	$id = check_plain(arg(3));
    	
    	$deleted = db_query("DELETE FROM {timetracker_project_events} WHERE id=%d", array($id));
		  
		$total_hours = 0;
    	
    	/* clean input */
    	$date = check_plain(arg(4));
    	
    	$all = db_query("SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.event_date = '%s'", array($user->uid,$date));
				
		
		while($record = db_fetch_object($all))
		{
			$total_hours += $record->project_hours;
		}
    	
    	print $date . '}{' . $total_hours;
    }
    
    /*  the response to the mini-form for updating an entry */
    
    function update_mytime()
    {
    	
    	global $user; 
    	
    	/* create select list of projects */
		// get vocabulary as options list
		$voc = variable_get('timetracker_projects', NULL);
		$tree = taxonomy_get_tree($voc,0,NULL,true);
		
    	
    	/* id title hours date */
    	$pid = check_plain($_POST['pid']);
    	$id = check_plain($_POST['id']);
    	$hours = check_plain($_POST['hours']);
    	$task = check_plain($_POST['task']);
    	
    	/*  get the term name */
    	$project = check_plain($_POST['project']);
		$project_info = taxonomy_get_term($project);
		
		//$sql = "UPDATE {timetracker_project_events} SET (project_user_task = '" . $task . "'," . "project_name = '" . $project_info_name  . "'," . "project_id = " . $project_info->tid . "," . "project_hours = $hours" . ","  . "changed = ". date("Y-m-d H:i:s") . " WHERE id = $id AND user_id = $user->uid";
		//print $sql;
				
		$event_update = db_query("UPDATE {timetracker_project_events} SET 
				project_user_task ='%s'," . 
				"project_name = '%s'," . 
				"project_id = %d," . 
				"project_hours = %d" . ","  .
				"changed = '%d'" . 
				" WHERE id = %d AND user_id = %d", array($task,$project_info->name,$project_info->tid,$hours,date("Y-m-d H:i:s"),$id,$user->uid));
		  
		if($event_update == TRUE)
		{
		 
		  /* limit to five days */
    	$all = db_fetch_object(db_query("SELECT * FROM {timetracker_project_events} e WHERE e.id = %d LIMIT 1", array($id)));
			
		
			
		
			$daily = '<tr class="event-added row-' . $all->id . '">' .
					  '<td class="cell-1-' . $all->id . '">' . $all->id . '</td>' . 
					  '<td class="cell-2-' . $all->id . '">' . $all->project_user_task . '</td>' . 
					  '<td class="cell-3-' . $all->id . '">' . $all->project_name . '</td>' . 
					  '<td class="event-date cell-4-' . $all->id . '">' . $all->event_date . '</td>' .
					  '<td class="cell-5-' . $all->id . '">' . $all->project_hours . '</td>' .
					  '<td class="cell-6-' . $all->id . '">' .
					  	l(t('delete'), 'timetracker/timetracker/delete_time/' . $all->id . '/' . $all->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $all->id))) . '</td>' .
					  '<td class="cell-7-' . $all->id . '">' .
						l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$all->id, array('attributes' => array('class' => array('update-time'), 'id' => 'upd-' . $all->id))) . '</td>' .
					  '</tr>';
					  
			$recent = '<tr class="event-added recent-'. $all->id .'" id="recent-' . $all->id . '">' .
					  '<td class="cell-1-' . $all->id . '">' . $all->id . '</td>' . 
					  '<td class="cell-2-' . $all->id . '">' . $all->project_user_task . '</td>' . 
					  '<td class="cell-3-' . $all->id . '">' . $all->project_name . '</td>' . 
					  '<td class="event-date cell-4-' . $all->id . '"><a href="#' . $all->event_date . '">' . $all->event_date . '</a></td>' .
					  '<td class="cell-5-' . $all->id . '">' . $all->project_hours . '</td>' .
					  '<td class="cell-6-' . $all->id . '">' .
					  	l(t('delete'), 'timetracker/timetracker/delete_time/' . $all->id . '/' . $all->project_id, array('attributes' => array('class' => array('delete-time'), 'id' => 'del-' . $all->id))) . '</td>' .
					  '<td class="cell-7-' . $all->id . '">' .
						l(t('update'), APP_MODULE .'/timetracker/update_mytime/'.$all->id, array('attributes' => array('class' => array('update-recent-time'), 'id' => 'upd-' . $all->id))) . '</td>' .
					  '</tr>';
					  
			$row_id = $all->id;
			
		/* update the hours total for the day */
		
		
		$all_hours = db_query("SELECT * FROM {timetracker_project_events} e WHERE e.user_id = %d AND e.event_date = '%s'", array($user->uid,$all->event_date));
				
		
		
		while($record = db_fetch_object($all_hours))
		{
			$total_hours += $record->project_hours;
		}	
		
    	
    	/* ajax response */
    	print $all->id . '}{' . $daily . '}{' . $total_hours . '}{' . $all->event_date . '}{' . $recent;
    	  
    	}
    	else
    	{
    		print t('error row not updated');
    	}
    }
    
    
    
}

?>
